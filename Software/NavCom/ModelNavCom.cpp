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
            m_uiMaxActive = 107900;
            break;
        case utCom1:
        case utCom2:
            m_uiMinActive = 118000;
            m_uiMaxActive = 136990;
            break;
        case utVOR1:
        case utVOR2:
            m_uiMinActive = 108000;
            m_uiMaxActive = 117950;
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

uint32_t ModelNavCom::IncrementDigit(uint32_t uiValue, int32_t iIncr, uint32_t uiOver)
{
    uint32_t uiMax = uiOver - abs(iIncr);
    uint32_t uiMod = pow(10,ceil(log10(uiOver)));

    if(iIncr > 0 && uiValue % uiMod >= uiMax) {
        uiValue -= uiOver;
    } else if(iIncr < 0 && uiValue % uiMod < abs(iIncr)) {
        uiValue += uiOver;
    }
    uiValue += iIncr;

    return uiValue;
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

void ModelNavCom::IncrActiveValue(int32_t iIncrement, uint32_t uiOverflow)
{
    if(m_pSetNavCom) {
        uint32_t uiTmp;
        if(uiOverflow != 0) {
            uiTmp = IncrementDigit(m_pSetNavCom->GetActiveValue(), iIncrement, uiOverflow);
        } else {
            uiTmp = m_pSetNavCom->GetActiveValue() + iIncrement;
        }
        uiTmp = helper::GetMinMax(uiTmp, m_uiMinActive, m_uiMaxActive);
        m_pSetNavCom->SetActiveValue(uiTmp);
    }
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

void ModelNavCom::IncrStandbyValue(int32_t iIncrement, uint32_t uiOverflow)
{
    if(m_pSetNavCom) {
        uint32_t uiTmp;
        if(uiOverflow != 0) {
            uiTmp = IncrementDigit(m_pSetNavCom->GetStandbyValue(), iIncrement, uiOverflow);
        } else {
            uiTmp = m_pSetNavCom->GetStandbyValue() + iIncrement;
        }
        uiTmp = helper::GetMinMax(uiTmp, m_uiMinActive, m_uiMaxActive);
        m_pSetNavCom->SetStandbyValue(uiTmp);
    }
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

void ModelNavCom::SetEditingDigit(std::pair<int16_t, int16_t> interval)
{
    m_pairEditingDigit = interval;
    m_uiValueEditTimestamp = helper::GetTime();
}

//-----------------------------------------------------------------------------

std::pair<int16_t, int16_t> ModelNavCom::GetEditingDigit() const
{
    // Return (1,0) = editing disabled after 3s of inactivity
    if (helper::GetTimeDifference(m_uiValueEditTimestamp) > 2000) {
        return std::make_pair(1,0);
    } else {
        return m_pairEditingDigit;
    }
}

//-----------------------------------------------------------------------------

void ModelNavCom::SetEditingFinished()
{
   m_pairEditingDigit = std::make_pair(1,0);
}

//-----------------------------------------------------------------------------
