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
