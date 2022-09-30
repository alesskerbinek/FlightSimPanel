#include "NetworkNavCom.h"

// -------------------------------------------------------------------------

NetworkNavCom::NetworkNavCom() : Network()
{
    m_pModel = nullptr;
}

// -------------------------------------------------------------------------

void NetworkNavCom::Initialize()
{
    m_pModel = ModelNavCom::GetInstance();

    Network::Initialize();
}

//-----------------------------------------------------------------------------

void NetworkNavCom::GetNetworkParameters(types::NetworkParameters* pParam)
{
    if(m_pModel && pParam) {
        pParam->m_iUdpListenPort = m_pModel->GetSettings()->GetListenPort();
        pParam->m_iUdpRemotePort = m_pModel->GetSettings()->GetRemotePort();
        pParam->m_uiUdpRemoteIP  = m_pModel->GetSettings()->GetRemoteIP();
        strncpy(pParam->m_acWiFiSSID, m_pModel->GetSettings()->GetWiFiSSID(), sizeof(pParam->m_acWiFiSSID));
        strncpy(pParam->m_acWiFiPass, m_pModel->GetSettings()->GetWiFiPass(), sizeof(pParam->m_acWiFiPass));
    }
}

// ----------------------------------------------------------------------------

void NetworkNavCom::ParseDREF(uint8_t* pBuffer)
{
    if(strncmp((char*)pBuffer+9, GetDataRefString(xplane::drCom1ActiveFreq), 51) == 0) { // TODO size??????
        float fValue = 0.0f;
        memcpy(&fValue, &(pBuffer[5]), sizeof(fValue));
        m_pModel->SetActiveValue(fValue);
    }
    else if(strncmp((char*)pBuffer+9, GetDataRefString(xplane::drCom1StandbyFreq), 59) == 0) { // TODO size??????
        float fValue = 0.0f;
        memcpy(&fValue, &(pBuffer[5]), sizeof(fValue));
        m_pModel->SetStandbyValue(fValue);
    }
}

// ----------------------------------------------------------------------------

void NetworkNavCom::ParseCOM(uint8_t* pBuffer)
{
    float fValue = 0.0f;
    memcpy(&fValue, &(pBuffer[4]), sizeof(fValue));
    //Serial.printf("Com1 act %i\n", int(fValue*10));
    memcpy(&fValue, &(pBuffer[8]), sizeof(fValue));
    //Serial.printf("Com1 stb %i\n", int(fValue*10));

    memcpy(&fValue, &(pBuffer[16]), sizeof(fValue));
    //Serial.printf("Com2 act %i\n", int(fValue*10));
    memcpy(&fValue, &(pBuffer[20]), sizeof(fValue));
    //Serial.printf("Com2 stb %i\n", int(fValue*10));
}

// ----------------------------------------------------------------------------

void NetworkNavCom::ParseNAV(uint8_t* pBuffer)
{
    float fValue = 0.0f;
    memcpy(&fValue, &(pBuffer[4]), sizeof(fValue));
    //Serial.printf("Nav1 act %i\n", int(fValue*10));
//    m_pModel->SetActiveValue(fValue*10);
    memcpy(&fValue, &(pBuffer[8]), sizeof(fValue));
    //Serial.printf("Nav1 stb %i\n", int(fValue*10));
//    m_pModel->SetStandbyValue(fValue*10);

    memcpy(&fValue, &(pBuffer[20]), sizeof(fValue));
    //Serial.printf("Nav2 act %i\n", int(fValue*10));
    memcpy(&fValue, &(pBuffer[24]), sizeof(fValue));
    //Serial.printf("Nav2 stb %i\n", int(fValue*10));
}

// ----------------------------------------------------------------------------

void NetworkNavCom::ParseOBS(uint8_t* pBuffer)
{
    float fValue = 0.0f;
    memcpy(&fValue, &(pBuffer[4]), sizeof(fValue));
    //Serial.printf("Nav1 obs %i\n", int(fValue*1000));

    memcpy(&fValue, &(pBuffer[20]), sizeof(fValue));
    //Serial.printf("Nav2 obs %i\n", int(fValue*1000));
}

// ----------------------------------------------------------------------------

void NetworkNavCom::ProcessTxQueue()
{
    if(m_pModel && m_pModel->IsTxQueueEmpty() == false)
    {
        xplane::UdpDatagram sDatagram = m_pModel->GetFromTxQueue();

        switch (sDatagram.m_eType) {
        case xplane::dtDATA:
            // Implement if needed...
            break;
        case xplane::dtDREF:
            SendDataRef(sDatagram.m_uiValue, static_cast<xplane::DataRefs>(sDatagram.m_uiParameter));
            break;
        case xplane::dtCHAR:
            SendChar(sDatagram.m_uiValue & 0xFF);
            break;
        case xplane::dtCMND:
            SendCommand(static_cast<xplane::Commands>(sDatagram.m_uiParameter));
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------
