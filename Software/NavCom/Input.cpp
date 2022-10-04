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
    if(m_pModel) {
        switch (m_pModel->GetUnitType()) {
        case utCom1:
        case utCom2:
        case utVOR1:
        case utVOR2:
            CheckRotaries(true, true);
            CheckButtons();
            break;
        case utADF1:
        case utADF2:
            CheckRotaries(false, true);
            CheckButtons();
            // TODO CheckSwitches();
            break;
        case utXPNDR:
            CheckRotaries(false, true);
            CheckButtons();
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------

void Input::CheckButtons()
{
    if(m_uiUsedButtons <= BUTTON_COUNT)
    {
        // Loop through all buttons
        for(uint8_t ui = 0; ui<m_uiUsedButtons; ui++)
        {
            // On press count up how long it is pressed
            if(digitalRead(m_auiButtonAddresses[ui]) == false) {
                m_auiButtonValues[ui]++;
                // Check for long-press
                if(m_auiButtonValues[ui] > 1000) {
                    HandleButtonRelease(ui, true);
                    m_auiButtonValues[ui] = 0;
                }
            } else {
                // On release check if it was pressed long enough to make action
                if(m_auiButtonValues[ui] > 50) {
                    HandleButtonRelease(ui, false);
                }
                m_auiButtonValues[ui] = 0;
            }
        }
    }
}

// -------------------------------------------------------------------------

void Input::CheckRotaries(bool bLeft, bool bRight)
{
    if(ROTARY_COUNT >= 2)
    {
        // In case rotary becomes erratic - jumping, try to increase minimum time difference since last change

        if(bRight) {
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
        }

        if(bLeft) {
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
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScroll(int8_t iRotaryId, int8_t iDirection)
{
    if(m_pModel) {
        switch (m_pModel->GetUnitType()) {
        case utCom1:
            HandleRotaryScrollCOM(iRotaryId, iDirection, 1); break;
        case utCom2:
            HandleRotaryScrollCOM(iRotaryId, iDirection, 2); break;
        case utVOR1:
            HandleRotaryScrollVOR(iRotaryId, iDirection, 1); break;
        case utVOR2:
            HandleRotaryScrollVOR(iRotaryId, iDirection, 2); break;
        case utADF1:
            HandleRotaryScrollADF(iRotaryId, iDirection, 1); break;
        case utADF2:
            HandleRotaryScrollADF(iRotaryId, iDirection, 2); break;
        case utXPNDR:
            HandleRotaryScrollXPNDR(iRotaryId, iDirection);
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonRelease(int8_t iButtonId, bool bIsLong)
{
    if(m_pModel) {
        switch (m_pModel->GetUnitType()) {
        case utCom1:
            HandleButtonReleaseCOM(iButtonId, 1, bIsLong); break;
        case utCom2:
            HandleButtonReleaseCOM(iButtonId, 2, bIsLong); break;
        case utVOR1:
            HandleButtonReleaseVOR(iButtonId, 1, bIsLong); break;
        case utVOR2:
            HandleButtonReleaseVOR(iButtonId, 2, bIsLong); break;
        case utADF1:
            HandleButtonReleaseADF(iButtonId, 1, bIsLong); break;
        case utADF2:
            HandleButtonReleaseADF(iButtonId, 2, bIsLong); break;
        case utXPNDR:
            HandleButtonReleaseXPNDR(iButtonId, bIsLong); break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollCOM(int8_t iRotaryId, int8_t iDirection, int8_t iComId)
{
    if(m_pModel && iRotaryId == 0) // Right rotary - Frequency
    {
        if(m_uiDigitSelect == 1) {
            xplane::Commands cmd;
            if(iComId == 1) {
                cmd = iDirection > 0 ? xplane::Commands::cmCom1StandbyCoarseUp : xplane::Commands::cmCom1StandbyCoarseDown;
            } else {
                cmd = iDirection > 0 ? xplane::Commands::cmCom2StandbyCoarseUp : xplane::Commands::cmCom2StandbyCoarseDown;
            }
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
        } else {
            xplane::Commands cmd;
            if(iComId == 1) {
                cmd = iDirection > 0 ? xplane::Commands::cmCom1Standby833Up : xplane::Commands::cmCom1Standby833Down;
            } else {
                cmd = iDirection > 0 ? xplane::Commands::cmCom2Standby833Up : xplane::Commands::cmCom2Standby833Down;
            }
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
        }
    }
    else if (m_pModel && iRotaryId == 1) // Left rotary - Volume
    {
        // TODO Volume
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollVOR(int8_t iRotaryId, int8_t iDirection, int8_t iVorId)
{
    if(m_pModel && iRotaryId == 0) // Right rotary - Frequency
    {
        if(m_uiDigitSelect == 1) {
            xplane::Commands cmd;
            if(iVorId == 1) {
                cmd = iDirection > 0 ? xplane::Commands::cmNav1StandbyCoarseUp : xplane::Commands::cmNav1StandbyCoarseDown;
            } else {
                cmd = iDirection > 0 ? xplane::Commands::cmNav1StandbyCoarseUp : xplane::Commands::cmNav2StandbyCoarseDown;
            }
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            //m_pModel->IncrStandbyValue(iDir*1000);
        } else {
            xplane::Commands cmd;
            if(iVorId == 1) {
                cmd = iDirection > 0 ? xplane::Commands::cmNav1StandbyFineUp : xplane::Commands::cmNav1StandbyFineDown;
            } else {
                cmd = iDirection > 0 ? xplane::Commands::cmNav1StandbyFineUp : xplane::Commands::cmNav2StandbyFineDown;
            }
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            //m_pModel->IncrStandbyValue(iDir*5);
        }
    }
    else if (m_pModel && iRotaryId == 1) // Left rotary - OBS
    {
        xplane::Commands cmd;
        if(iVorId == 1) {
            cmd = iDirection > 0 ? xplane::Commands::cmNav1ObsUp : xplane::Commands::cmNav1ObsDown;
        } else {
            cmd = iDirection > 0 ? xplane::Commands::cmNav1ObsUp : xplane::Commands::cmNav2ObsDown;
        }
        m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollADF(int8_t iRotaryId, int8_t iDirection, int8_t iAdfId)
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

void Input::HandleRotaryScrollXPNDR(int8_t iRotaryId, int8_t iDirection)
{
    if(m_pModel)
    {
        if(m_uiDigitSelect == 0) {
            // TODO change X000
            m_pModel->SetActiveValue(m_pModel->GetActiveValue()+iDirection*1000);
        } else if(m_uiDigitSelect == 1) {
            // TODO change 0X00
            m_pModel->SetActiveValue(m_pModel->GetActiveValue()+iDirection*100);
        } else if(m_uiDigitSelect == 2) {
            // TODO change 00X0
            m_pModel->SetActiveValue(m_pModel->GetActiveValue()+iDirection*10);
        } else {
            // TODO change 000X
            m_pModel->SetActiveValue(m_pModel->GetActiveValue()+iDirection);
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonReleaseCOM(int8_t iButtonId, int8_t iComId, bool bIsLong)
{
    /*
    AddToTxQueue(UdpDatagram(UdpDataType::dtCHAR, 0, (uint32_t)0x41));
    */

    switch (iButtonId) {
    case 0:
        m_uiDigitSelect = (m_uiDigitSelect+1)%2;
        break;
    case 2: // On/Off
        if(m_pModel) {
            //m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmCom1Off));
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtDREF,
                        iComId == 1 ? xplane::DataRefs::drCom1Power : xplane::DataRefs::drCom1Power,
                        (uint32_t)0)); // == Off
        }
        break;
    case 3: // Flip
        if(m_pModel) {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                        iComId == 1 ? xplane::Commands::cmCom1Flip : xplane::Commands::cmCom2Flip));
            m_uiDigitSelect = 0;
        }
        break;
    case 4: // Set Emergency frequency
        if(m_pModel) {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtDREF,
                        iComId == 1 ? xplane::DataRefs::drCom1ActiveFreq : xplane::DataRefs::drCom2ActiveFreq,
                        (uint32_t)0x47ed4e00)); // == 121.500
        }
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonReleaseVOR(int8_t iButtonId, int8_t iVorId, bool bIsLong)
{
    switch (iButtonId) {
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
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iVorId == 1 ? xplane::Commands::cmNav1Flip : xplane::Commands::cmNav2Flip));
                m_uiDigitSelect = 0;
//            }
        }
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonReleaseADF(int8_t iButtonId, int8_t iAdfId, bool bIsLong)
{
    switch (iButtonId) {
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
        if(m_pModel) {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                        iAdfId == 1 ? xplane::Commands::cmAdf1Flip : xplane::Commands::cmAdf2Flip));
            m_uiDigitSelect = 0;
        }
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

void Input::HandleButtonReleaseXPNDR(int8_t iButtonId, bool bIsLong)
{
    switch (iButtonId) {
    case 0: // Go to next digit
        m_uiDigitSelect = (m_uiDigitSelect+1)%4;
        break;
    case 1: // Ident
        m_pModel->SetXpndrIdent();
        break;
    case 2: // ALT
        m_pModel->SetXpndrMode(xmAlt);
        break;
    case 3:// ON
        m_pModel->SetXpndrMode(xmOn);
        break;
    case 4: // SBY
        m_pModel->SetXpndrMode(xmSby);
        break;
    case 5: // OFF
        if(bIsLong) {
            m_pModel->SetXpndrMode(xmOff);
        }
        break;
    case 6: // VFR (Set squawk 2000)
        m_pModel->SetActiveValue(2000);
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------
