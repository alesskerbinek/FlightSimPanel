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
        case utCom:
            m_uiMinActive = 118000;
            m_uiMaxActive = 136000;
            break;
        case utVOR:
            m_uiMinActive = 108000;
            m_uiMaxActive = 118000;
            break;
        case utADF:
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