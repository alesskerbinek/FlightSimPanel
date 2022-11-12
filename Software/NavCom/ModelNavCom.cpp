#include <Helper.h>
#include "ModelNavCom.h"

ModelNavCom* ModelNavCom::s_pInstanceNavCom = NULL;

// ----------------------------------------------------------------------------

ModelNavCom::ModelNavCom() : Model()
{

}

//-----------------------------------------------------------------------------

ModelNavCom* ModelNavCom::GetInstance()
{
    if (s_pInstanceNavCom == NULL) {
            s_pInstanceNavCom = new ModelNavCom();
            s_pInstance = s_pInstanceNavCom;
    }

    return s_pInstanceNavCom;
}

// -------------------------------------------------------------------------

void ModelNavCom::Initialize()
{
    if(m_pSetNavCom) {
        switch (GetUnitType()) {
        case utUndefined:
            m_uiMinActive = 94000;
            m_uiMaxActive = 108000;
            break;
        case utCom1:
        case utCom2:
            m_uiMinActive = 118000;
            m_uiMaxActive = 136000;
            break;
        case utVOR1:
        case utVOR2:
            m_uiMinActive = 108000;
            m_uiMaxActive = 118000;
            break;
        case utADF1:
        case utADF2:
            m_uiMinActive = 190;
            m_uiMaxActive = 1750;
            break;
        case utXPNDR:
            m_uiMinActive = 0;
            m_uiMaxActive = 7777;
            break;
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

uint32_t ModelNavCom::GetActiveValue() const
{
    return m_pSetNavCom ? m_pSetNavCom->GetActiveValue() : 0;
}

//-----------------------------------------------------------------------------

void ModelNavCom::SetActiveValue(uint32_t uiValue)
{
    uiValue = helper::GetMinMax(uiValue, m_uiMinActive, m_uiMaxActive);
    if(m_pSetNavCom) m_pSetNavCom->SetActiveValue(uiValue);
}

//-----------------------------------------------------------------------------

uint32_t ModelNavCom::GetStandbyValue() const
{
    return m_pSetNavCom ? m_pSetNavCom->GetStandbyValue() : 0;
}

//-----------------------------------------------------------------------------

void ModelNavCom::SetStandbyValue(uint32_t uiValue)
{
    uiValue = helper::GetMinMax(uiValue, m_uiMinActive, m_uiMaxActive);
    if(m_pSetNavCom) m_pSetNavCom->SetStandbyValue(uiValue);
}

//-----------------------------------------------------------------------------

void ModelNavCom::IncrStandbyValue(int32_t iIncrement)
{
    uint32_t uiTmp = m_pSetNavCom->GetStandbyValue() + iIncrement;
    uiTmp = helper::GetMinMax(uiTmp, m_uiMinActive, m_uiMaxActive);
    if(m_pSetNavCom) m_pSetNavCom->SetStandbyValue(uiTmp);
}

//-----------------------------------------------------------------------------

void ModelNavCom::SetXpndrIdent()
{
    m_uiXpndrIdentTimestamp = helper::GetTime();
}

//-----------------------------------------------------------------------------

bool ModelNavCom::IsXpndrIdent()
{
    return (m_uiXpndrIdentTimestamp > 0 && helper::GetTimeDifference(m_uiXpndrIdentTimestamp) < 5000);
}

//-----------------------------------------------------------------------------

void ModelNavCom::SetXpndrShutdown(bool bShutdown)
{
    m_bXpndrShutdown = bShutdown;
}

//-----------------------------------------------------------------------------

bool ModelNavCom::IsXpndrShutdown()
{
    return m_bXpndrShutdown;
}

//-----------------------------------------------------------------------------

uint8_t ModelNavCom::GetVolume() const
{
    return m_uiVolume;
}

//-----------------------------------------------------------------------------

void ModelNavCom::SetVolume(int8_t iValue)
{
    m_uiVolume = helper::GetMinMax(int(iValue), 0, 100);
    m_uiVolumeTimestamp = helper::GetTime();
}

//-----------------------------------------------------------------------------

bool ModelNavCom::IsVolumeCommand()
{
    return (m_uiVolumeTimestamp > 0 && helper::GetTimeDifference(m_uiVolumeTimestamp) < 2000);
}

//-----------------------------------------------------------------------------
