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

void NetworkNavCom::ParseDREF(uint8_t* pBuffer)
{
    float fValue = 0.0f;
    if(m_pModel && m_pModel->GetUnitType() == utCom1)
    {
        if(strncmp((char*)pBuffer+9, GetDataRefString(xplane::drCom1ActiveFreq), 51) == 0) { // TODO size??????
            memcpy(&fValue, &(pBuffer[5]), sizeof(fValue));
            m_pModel->SetActiveValue(fValue);
        }
        else if(strncmp((char*)pBuffer+9, GetDataRefString(xplane::drCom1StandbyFreq), 59) == 0) { // TODO size??????
            memcpy(&fValue, &(pBuffer[5]), sizeof(fValue));
            m_pModel->SetStandbyValue(fValue);
        }
    }
    else if(m_pModel && m_pModel->GetUnitType() == utCom2)
    {
        if(strncmp((char*)pBuffer+9, GetDataRefString(xplane::drCom2ActiveFreq), 51) == 0) { // TODO size??????
            memcpy(&fValue, &(pBuffer[5]), sizeof(fValue));
            m_pModel->SetActiveValue(fValue);
        }
        else if(strncmp((char*)pBuffer+9, GetDataRefString(xplane::drCom2StandbyFreq), 59) == 0) { // TODO size??????
            memcpy(&fValue, &(pBuffer[5]), sizeof(fValue));
            m_pModel->SetStandbyValue(fValue);
        }
    }
}

// ----------------------------------------------------------------------------

void NetworkNavCom::ParseCOM(uint8_t* pBuffer)
{
    // DEPRECATED - we use DREF which supports 8.33 kHz
    return;

    float fValue = 0.0f;
    if(m_pModel && m_pModel->GetUnitType() == utCom1)
    {
        memcpy(&fValue, &(pBuffer[4]), sizeof(fValue));     // COM 1 Active Freq
        memcpy(&fValue, &(pBuffer[8]), sizeof(fValue));     // COM 1 Standby Freq
    }
    else if(m_pModel && m_pModel->GetUnitType() == utCom2)
    {
        memcpy(&fValue, &(pBuffer[16]), sizeof(fValue));     // COM 2 Active Freq
        memcpy(&fValue, &(pBuffer[20]), sizeof(fValue));     // COM 2 Standby Freq
    }
}

// ----------------------------------------------------------------------------

void NetworkNavCom::ParseNAV(uint8_t* pBuffer)
{
    float fValue = 0.0f;
    if(m_pModel && m_pModel->GetUnitType() == utVOR1)
    {
        memcpy(&fValue, &(pBuffer[4]), sizeof(fValue));     // NAV 1 Active Freq
        m_pModel->SetActiveValue(fValue*10);
        memcpy(&fValue, &(pBuffer[8]), sizeof(fValue));     // NAV 1 Standby Freq
        m_pModel->SetStandbyValue(fValue*10);
    }
    else if(m_pModel && m_pModel->GetUnitType() == utVOR2)
    {
        memcpy(&fValue, &(pBuffer[20]), sizeof(fValue));     // NAV 2 Active Freq
        m_pModel->SetActiveValue(fValue*10);
        memcpy(&fValue, &(pBuffer[24]), sizeof(fValue));     // NAV 2 Standby Freq
        m_pModel->SetStandbyValue(fValue*10);
    }
}

// ----------------------------------------------------------------------------

void NetworkNavCom::ParseOBS(uint8_t* pBuffer)
{    
    float fValue = 0.0f;
    if(m_pModel && m_pModel->GetUnitType() == utVOR1)
    {
        memcpy(&fValue, &(pBuffer[4]), sizeof(fValue));         // NAV 1 OBS
        m_pModel->GetSettings()->SetOBS(fValue);
    }
    else if(m_pModel && m_pModel->GetUnitType() == utVOR2)
    {
        memcpy(&fValue, &(pBuffer[20]), sizeof(fValue));        // NAV 2 OBS
        m_pModel->GetSettings()->SetOBS(fValue);
    }
}

// ----------------------------------------------------------------------------
