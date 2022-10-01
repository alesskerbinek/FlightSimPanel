#include "Input.h"
#include "Helper.h"
#include "XPlaneDef.h"
#include "ApplicationDefines.h"

// -------------------------------------------------------------------------

Input::Input()
{
    memset(m_auiButtonValues, 0, sizeof(m_auiButtonValues));
}

// -------------------------------------------------------------------------

void Input::Initialize()
{
    m_pModel = ModelNavCom::GetInstance();

    if(ROTARY_COUNT >= 2) {
        m_abLastState[0] = digitalRead(ROT1_A);
        m_abLastState[1] = digitalRead(ROT2_A);
        m_auiLastChange[0] = helper::GetTime();
        m_auiLastChange[1] = helper::GetTime();
    }
}

// -------------------------------------------------------------------------

void Input::Process()
{
    CheckRotaries();
    CheckButtons();
}

// -------------------------------------------------------------------------

void Input::CheckButtons()
{
    if(m_uiUsedButtons <= BUTTON_COUNT)
    {
        for(uint8_t ui = 0; ui<m_uiUsedButtons; ui++)
        {
            if(digitalRead(m_auiButtonAddresses[ui]) == false) {
                m_auiButtonValues[ui]++;
            } else {
                // TODO if(m_auiButtonValues[ui] > 500) {
                //          HandleButtonReleaseLong(ui);
                if(m_auiButtonValues[ui] > 50) {
                    HandleButtonRelease(ui);
                }
                m_auiButtonValues[ui] = 0;
            }
        }
    }
}

// -------------------------------------------------------------------------

void Input::CheckRotaries()
{
    if(ROTARY_COUNT >= 2)
    {
        // In case rotary becomes erratic - jumping, try to increase minimum time difference since last change

        bool bB1 = digitalRead(ROT1_B);
        if(bB1 != m_abLastState[0] && bB1 == 1 && helper::GetTimeDifference(m_auiLastChange[0]) > 50)
        {
            bool bA1 = digitalRead(ROT1_A);
            if(bA1 != bB1) {
                HandleRotaryScroll(0, -1);
            } else {
                HandleRotaryScroll(0, 1);
            }
            m_auiLastChange[0] = helper::GetTime();
        }
        m_abLastState[0] = bB1;

        bool bB2 = digitalRead(ROT2_B);
        if(bB2 != m_abLastState[1] && bB2 == 1 && helper::GetTimeDifference(m_auiLastChange[1]) > 50)
        {
            bool bA2 = digitalRead(ROT2_A);
            if(bA2 != bB2) {
                HandleRotaryScroll(1, 1);
            } else {
                HandleRotaryScroll(1, -1);
            }
            m_auiLastChange[1] = helper::GetTime();
        }
        m_abLastState[1] = bB2;
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScroll(int8_t iId, int8_t iDir)
{
    if(m_pModel) {
        switch (m_pModel->GetUnitType()) {
        case utCom:
            HandleRotaryScrollCOM(iId, iDir);
            break;
        case utVOR:
            HandleRotaryScrollVOR(iId, iDir);
            break;
        case utADF:
            HandleRotaryScrollADF(iId, iDir);
            break;
        case utXPNDR:
            HandleRotaryScrollXPNDR(iId, iDir);
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonRelease(int8_t iId)
{
    if(m_pModel) {
        switch (m_pModel->GetUnitType()) {
        case utCom:
            HandleButtonReleaseCOM(iId);
            break;
        case utVOR:
            HandleButtonReleaseVOR(iId);
            break;
        case utADF:
            HandleButtonReleaseADF(iId);
            break;
        case utXPNDR:
            HandleButtonReleaseXPNDR(iId);
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollCOM(int8_t iId, int8_t iDir)
{
    if(m_pModel && iId == 0) // Right rotary
    {
        if(m_uiDigitSelect == 1) {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, iDir > 0 ? xplane::Commands::cmCom1StandbyCoarseUp : xplane::Commands::cmCom1StandbyCoarseDown));
        } else {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, iDir > 0 ? xplane::Commands::cmCom1Standby833Up : xplane::Commands::cmCom1Standby833Down));
        }
    }
    else if (m_pModel && iId == 1) // Left rotary
    {
        // TODO Volume
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollVOR(int8_t iId, int8_t iDir)
{
    if(m_pModel && iId == 0) // Right rotary
    {
        if(m_uiDigitSelect == 1) {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, iDir > 0 ? xplane::Commands::cmNav1StandbyCoarseUp : xplane::Commands::cmNav1StandbyCoarseDown));
            //m_pModel->IncrStandbyValue(iDir*1000);
        } else {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, iDir > 0 ? xplane::Commands::cmNav1StandbyFineUp : xplane::Commands::cmNav1StandbyFineDown));
            //m_pModel->IncrStandbyValue(iDir*5);
        }
    }
    else if (m_pModel && iId == 1) // Left rotary
    {
        // TODO OBS
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollADF(int8_t iId, int8_t iDir)
{
    if(m_pModel)
    {
        if(m_uiDigitSelect == 0) {
            // TODO change XX00
        } else if(m_uiDigitSelect == 1) {
            // TODO change 00X0
        } else {
            // TODO change 000X
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollXPNDR(int8_t iId, int8_t iDir)
{
    if(m_pModel)
    {
        if(m_uiDigitSelect == 0) {
            // TODO change X000
        } else if(m_uiDigitSelect == 1) {
            // TODO change 0X00
        } else if(m_uiDigitSelect == 2) {
            // TODO change 00X0
        } else {
            // TODO change 000X
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonReleaseCOM(int8_t iId)
{
    /*
    AddToTxQueue(UdpDatagram(UdpDataType::dtDREF, DataRefs::drCom1Freq, (uint32_t)0x462AA001));
    AddToTxQueue(UdpDatagram(UdpDataType::dtCHAR, 0, (uint32_t)0x41));
    AddToTxQueue(UdpDatagram(UdpDataType::dtCMND, Commands::cmLandingGear));
    */

    switch (iId) {
    case 0:
        m_uiDigitSelect = (m_uiDigitSelect+1)%2;
        break;
    case 1:
        // TODO On/Off ???
        break;
    case 3:
        if(m_pModel) {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmCom1Flip));
            m_uiDigitSelect = 0;
        }
        break;
    case 4:
        // TODO Emergency
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonReleaseVOR(int8_t iId)
{
    switch (iId) {
    case 0:
        m_uiDigitSelect = (m_uiDigitSelect+1)%2;
        break;
    case 1:
        // TODO Show OBS
        break;
    case 3:
        if(m_pModel) {
//            if(m_pModel->IsDemoMode()) {
//                uint32_t uiTmp = m_pModel->GetActiveValue();
//                m_pModel->SetActiveValue(m_pModel->GetStandbyValue());
//                m_pModel->SetStandbyValue(uiTmp);
//                m_pModel->SaveSettings();
//            } else {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmNav1Flip));
                m_uiDigitSelect = 0;
//            }
        }
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonReleaseADF(int8_t iId)
{
    switch (iId) {
    case 0:
        m_uiDigitSelect = (m_uiDigitSelect+1)%3;
        break;
    case 2:
        // TODO SET/RST
        break;
    case 3:
        // TODO FT/ET
        break;
    case 4:
        // TODO <->/FREQ
        break;
    case 5:
        // TODO BFO ???
        break;
    case 6:
        // TODO ADF
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonReleaseXPNDR(int8_t iId)
{
    switch (iId) {
    case 0:
        m_uiDigitSelect = (m_uiDigitSelect+1)%4;
        break;
    case 2:
        // TODO ALT
        break;
    case 3:
        // TODO ON
        break;
    case 4:
        // TODO SBY
        break;
    case 5:
        // TODO OFF
        break;
    case 6:
        // TODO VFR
        break;
    case 7:
        // TODO IDT
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------
