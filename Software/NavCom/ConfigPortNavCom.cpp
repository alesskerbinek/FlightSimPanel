#include "ApplicationDefines.h"
#include "ConfigPortNavCom.h"

//-----------------------------------------------------------------------------

ConfigPortNavCom::ConfigPortNavCom() : ConfigPort()
{
    m_pModel = nullptr;
}

// ----------------------------------------------------------------------------

void ConfigPortNavCom::Initialize()
{
    m_pModel = ModelNavCom::GetInstance();

    Serial.println();

    std::array<char, MAX_NMEA_SIZE-1> acBuff;
    snprintf(acBuff.data(), acBuff.size(), "PMDBG,%s unit started,v%d.%d.%d",
             PROJECT_NAME, SW_VERSION_MAJOR, SW_VERSION_MINOR, BUILD_NUMBER);
    Send(static_cast<const char*>(acBuff.data()), false);

    snprintf(acBuff.data(), acBuff.size(), "PINIT,SET,%ld,%d",
             m_pModel->GetSettings()->GetSerialNumber(), m_pModel->GetSettings()->GetUnitType());
    Send(static_cast<const char*>(acBuff.data()), true);
}

//-----------------------------------------------------------------------------

void ConfigPortNavCom::Process()
{
    // Read while there is something in the buffer
    uint8_t byte;
    while(Serial.available() && Serial.readBytes(&byte,1))
    {
        // Process byte
        ProcessByte(byte);
    }
}

// ----------------------------------------------------------------------------

void ConfigPortNavCom::GetProductionParameters(types::ProductionParameters* sParam)
{
    if(m_pModel && sParam) {
        sParam->m_iSerialNumber = m_pModel->GetSettings()->GetSerialNumber();
        sParam->m_iType = static_cast<uint8_t>(m_pModel->GetSettings()->GetUnitType());
    }
}

// ----------------------------------------------------------------------------

bool ConfigPortNavCom::SetProductionParameters(types::ProductionParameters* sParam)
{
    if(m_pModel == nullptr || sParam == nullptr)
        return false;

//    m_pModel->GetSettings()->InitializeAll();
    if(sParam->m_iSerialNumber >= 0) m_pModel->GetSettings()->SetSerialNumber(sParam->m_iSerialNumber);
    if(sParam->m_iType >= 0) m_pModel->GetSettings()->SetUnitType(static_cast<UnitTypes>(sParam->m_iType));

    m_pModel->SaveSettings();

    return true;
}

// ----------------------------------------------------------------------------

void ConfigPortNavCom::GetSystemParameters(types::SystemParameters* pParam)
{
    if(m_pModel && pParam) {
        pParam->m_uiLoopCount = m_pModel->GetLoopCount();
    }
}

// ----------------------------------------------------------------------------

bool ConfigPortNavCom::FactoryReset()
{
    if(m_pModel) {
        m_pModel->GetSettings()->Initialize();
        m_pModel->GetSettings()->Save();
        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------

void ConfigPortNavCom::GetNetworkParameters(types::NetworkParameters* sParam)
{
    if(m_pModel && sParam) {
        memcpy(sParam->m_acWiFiSSID, m_pModel->GetSettings()->GetWiFiSSID(), sizeof(sParam->m_acWiFiSSID));
        memcpy(sParam->m_acWiFiPass, m_pModel->GetSettings()->GetWiFiPass(), sizeof(sParam->m_acWiFiPass));
        sParam->m_iUdpListenPort = m_pModel->GetSettings()->GetListenPort();
        sParam->m_iUdpRemotePort = m_pModel->GetSettings()->GetRemotePort();
        sParam->m_uiUdpRemoteIP = m_pModel->GetSettings()->GetRemoteIP();
    }
}

// ----------------------------------------------------------------------------

bool ConfigPortNavCom::SetNetworkParameters(types::NetworkParameters* sParam)
{
    if(m_pModel == nullptr || sParam == nullptr)
        return false;

    if(sParam->m_acWiFiSSID[0] != '\0') {
        memcpy(m_pModel->GetSettings()->GetWiFiSSID(), sParam->m_acWiFiSSID, m_pModel->GetSettings()->GetWiFiSSIDSize());
    }
    if(sParam->m_acWiFiPass[0] != '\0') {
        memcpy(m_pModel->GetSettings()->GetWiFiPass(), sParam->m_acWiFiPass, m_pModel->GetSettings()->GetWiFiPassSize());
    }
    if(sParam->m_iUdpListenPort >= 0) m_pModel->GetSettings()->SetListenPort(sParam->m_iUdpListenPort);
    if(sParam->m_iUdpRemotePort >= 0) m_pModel->GetSettings()->SetRemotePort(sParam->m_iUdpRemotePort);
    if(sParam->m_uiUdpRemoteIP > 0)   m_pModel->GetSettings()->SetRemoteIP(sParam->m_uiUdpRemoteIP);

    m_pModel->SaveSettings();

    return true;
}

// ----------------------------------------------------------------------------
