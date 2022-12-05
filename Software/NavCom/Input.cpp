#include "Input.h"
#include "Helper.h"
#include "XPlaneDef.h"
#include "ApplicationDefines.h"

#define BTN_PRESS_CNT       50
#define BTN_LONGPRESS_CNT   1000

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
                // If button pressed for 50 cycles, we declare press event
                if(m_auiButtonValues[ui] == BTN_PRESS_CNT) {
                    HandleButtonEvent(ui, ButtonEvents::bePressed);
                }
                // If pressed for more than 1000 cycles, we declare long-press
                if(m_auiButtonValues[ui] > BTN_LONGPRESS_CNT) {
                    HandleButtonEvent(ui, ButtonEvents::beLongPress);
                    m_auiButtonValues[ui] = BTN_PRESS_CNT;
                }
            } else {
                // On release check if it was pressed long enough to declare release
                if(m_auiButtonValues[ui] > BTN_PRESS_CNT) {
                    HandleButtonEvent(ui, ButtonEvents::beReleased);
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
                    HandleRotaryScroll(1, -1);
                } else {
                    HandleRotaryScroll(1, 1);
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

void Input::HandleButtonEvent(int8_t iButtonId, ButtonEvents eEvent)
{
    if(m_pModel) {
        switch (m_pModel->GetUnitType()) {
        case utCom1:
            HandleButtonEventCOM(iButtonId, 1, eEvent); break;
        case utCom2:
            HandleButtonEventCOM(iButtonId, 2, eEvent); break;
        case utVOR1:
            HandleButtonEventVOR(iButtonId, 1, eEvent); break;
        case utVOR2:
            HandleButtonEventVOR(iButtonId, 2, eEvent); break;
        case utADF1:
            HandleButtonEventADF(iButtonId, 1, eEvent); break;
        case utADF2:
            HandleButtonEventADF(iButtonId, 2, eEvent); break;
        case utXPNDR:
            HandleButtonEventXPNDR(iButtonId, eEvent); break;
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
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                if(iComId == 1) {
                    cmd = iDirection > 0 ? xplane::Commands::cmCom1StandbyCoarseUp : xplane::Commands::cmCom1StandbyCoarseDown;
                } else {
                    cmd = iDirection > 0 ? xplane::Commands::cmCom2StandbyCoarseUp : xplane::Commands::cmCom2StandbyCoarseDown;
                }
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            } else {
                m_pModel->IncrStandbyValue(iDirection*1000, 0);
            }
        } else {
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                if(iComId == 1) {
                    cmd = iDirection > 0 ? xplane::Commands::cmCom1Standby833Up : xplane::Commands::cmCom1Standby833Down;
                } else {
                    cmd = iDirection > 0 ? xplane::Commands::cmCom2Standby833Up : xplane::Commands::cmCom2Standby833Down;
                }
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            } else {
                m_pModel->IncrStandbyValue(iDirection*5, 1000);
                // Skip invalid values: 20, 45, 70, 95!
                uint32_t uiCur = m_pModel->GetStandbyValue() % 100;
                if(uiCur == 20 || uiCur == 45 || uiCur == 70 || uiCur == 95) {
                    m_pModel->IncrStandbyValue(iDirection*5, 1000);
                }
            }
        }
        // Switch between (9,11) and (6,8)
        m_pModel->SetEditingDigit(9-m_uiDigitSelect*3,11-m_uiDigitSelect*3);
    }
    else if (m_pModel && iRotaryId == 1) // Left rotary - Volume
    {
        // This is just for display. No action is performed on simulator.
        m_pModel->SetVolume(m_pModel->GetVolume()+iDirection*5);
        m_pModel->SetEditingDigit(9,11);
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollVOR(int8_t iRotaryId, int8_t iDirection, int8_t iVorId)
{
    if(m_pModel && iRotaryId == 0) // Right rotary - Frequency
    {
        if(m_uiDigitSelect == 1) {
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                if(iVorId == 1) {
                    cmd = iDirection > 0 ? xplane::Commands::cmNav1StandbyCoarseUp : xplane::Commands::cmNav1StandbyCoarseDown;
                } else {
                    cmd = iDirection > 0 ? xplane::Commands::cmNav1StandbyCoarseUp : xplane::Commands::cmNav2StandbyCoarseDown;
                }
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            } else {
                m_pModel->IncrStandbyValue(iDirection*1000, 0);
            }
        } else {
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                if(iVorId == 1) {
                    cmd = iDirection > 0 ? xplane::Commands::cmNav1StandbyFineUp : xplane::Commands::cmNav1StandbyFineDown;
                } else {
                    cmd = iDirection > 0 ? xplane::Commands::cmNav1StandbyFineUp : xplane::Commands::cmNav2StandbyFineDown;
                }
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            } else {
                m_pModel->IncrStandbyValue(iDirection*50, 1000);
            }
        }
        // Switch between (9,11) and (6,8)
        m_pModel->SetEditingDigit(9-m_uiDigitSelect*3,11-m_uiDigitSelect*3);
    }
    else if (m_pModel && iRotaryId == 1) // Left rotary - OBS
    {
        if(m_pModel->IsSimConnected()) {
            xplane::Commands cmd;
            if(iVorId == 1) {
                cmd = iDirection > 0 ? xplane::Commands::cmNav1ObsUp : xplane::Commands::cmNav1ObsDown;
            } else {
                cmd = iDirection > 0 ? xplane::Commands::cmNav1ObsUp : xplane::Commands::cmNav2ObsDown;
            }
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
        } else {
            m_pModel->IncrStandbyValue(iDirection*100, 1000); // TODO
        }
        m_pModel->SetEditingFinished();
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollADF(int8_t /*iRotaryId*/, int8_t iDirection, int8_t iAdfId)
{
    if(m_pModel)
    {
        if(m_uiDigitSelect == 0) { // Change XX00
            if(m_pModel->IsSimConnected()) {
                // TODO
            } else {
                m_pModel->IncrStandbyValue(iDirection*100, 0);
            }
            m_pModel->SetEditingDigit(6,7);
        } else if(m_uiDigitSelect == 1) { // Change 00X0
            if(m_pModel->IsSimConnected()) {
                // TODO
            } else {
                m_pModel->IncrStandbyValue(iDirection*10, 100);
            }
            m_pModel->SetEditingDigit(8);
        } else { // Change 000X
            if(m_pModel->IsSimConnected()) {
                // TODO
            } else {
                m_pModel->IncrStandbyValue(iDirection, 10);
            }
            m_pModel->SetEditingDigit(9);
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollXPNDR(int8_t /*iRotaryId*/, int8_t iDirection)
{
    if(m_pModel)
    {
        if(m_uiDigitSelect == 0) { // Change X000
            if(m_pModel->IsSimConnected()) {
                // TODO
            } else {
                m_pModel->IncrActiveValue(iDirection*1000, 8000);
            }
            m_pModel->SetEditingDigit(6);
        } else if(m_uiDigitSelect == 1) { // Change 0X00
            if(m_pModel->IsSimConnected()) {
                // TODO
            } else {
                m_pModel->IncrActiveValue(iDirection*100, 800);
            }
            m_pModel->SetEditingDigit(7);
        } else if(m_uiDigitSelect == 2) { // Change 00X0
            if(m_pModel->IsSimConnected()) {
                // TODO
            } else {
                m_pModel->IncrActiveValue(iDirection*10, 80);
            }
            m_pModel->SetEditingDigit(8);
        } else { // Change 000X
            if(m_pModel->IsSimConnected()) {
                // TODO
            } else {
                m_pModel->IncrActiveValue(iDirection, 8);
            }
            m_pModel->SetEditingDigit(9);
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonEventCOM(int8_t iButtonId, int8_t iComId, ButtonEvents eEvent)
{
    /*
    AddToTxQueue(UdpDatagram(UdpDataType::dtCHAR, 0, (uint32_t)0x41)); // TODO remove if not working
    */

    switch (iButtonId) {
    case 0:
        if(eEvent == beReleased) {
            m_uiDigitSelect = (m_uiDigitSelect+1)%2;
            // Switch between (9,11) and (6,8)
            m_pModel->SetEditingDigit(9-m_uiDigitSelect*3,11-m_uiDigitSelect*3);
        }
        break;
    case 2: // On/Off
        if(eEvent == beReleased && m_pModel) {
            // TODO remove !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            if(m_pModel->IsSimConnected()) {
                // Does this work ???
                //m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmCom1Off));
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtDREF,
                        iComId == 1 ? xplane::DataRefs::drCom1Power : xplane::DataRefs::drCom1Power,
                        (uint32_t)0)); // == Off
            } else {
                // TODO
            }
        }
        break;
    case 3: // Flip
        if(eEvent == beReleased && m_pModel) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iComId == 1 ? xplane::Commands::cmCom1Flip : xplane::Commands::cmCom2Flip));
                m_uiDigitSelect = 0;
            } else {
                uint32_t uiTmp = m_pModel->GetActiveValue();
                m_pModel->SetActiveValue(m_pModel->GetStandbyValue());
                m_pModel->SetStandbyValue(uiTmp);
                //m_pModel->SaveSettings();
            }
            m_pModel->SetEditingFinished();
        }
        break;
    case 1: // Set Emergency frequency
        if(eEvent == beReleased && m_pModel) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtDREF,
                        iComId == 1 ? xplane::DataRefs::drCom1ActiveFreq : xplane::DataRefs::drCom2ActiveFreq,
                        (uint32_t)0x47ed4e00)); // == 121.500
            } else {
                m_pModel->SetActiveValue(121500);
            }
            m_pModel->SetEditingFinished();
        }
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonEventVOR(int8_t iButtonId, int8_t iVorId, ButtonEvents eEvent)
{
    Serial.print("BTN ID:");
    Serial.print(iButtonId);
    Serial.print("Event:");
    Serial.println(int(eEvent));

    switch (iButtonId) {
    case 0:
        if(eEvent == beReleased) {
            m_uiDigitSelect = (m_uiDigitSelect+1)%2;
            // Switch between (9,11) and (6,8)
            m_pModel->SetEditingDigit(9-m_uiDigitSelect*3,11-m_uiDigitSelect*3);
        }
        break;
    case 1:
        // TODO Show OBS
        break;
    case 3:
        if(eEvent == beReleased && m_pModel) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iVorId == 1 ? xplane::Commands::cmNav1Flip : xplane::Commands::cmNav2Flip));
                m_uiDigitSelect = 0;
            } else {
                uint32_t uiTmp = m_pModel->GetActiveValue();
                m_pModel->SetActiveValue(m_pModel->GetStandbyValue());
                m_pModel->SetStandbyValue(uiTmp);
                //m_pModel->SaveSettings();
            }
            m_pModel->SetEditingFinished();
        }
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonEventADF(int8_t iButtonId, int8_t iAdfId, ButtonEvents eEvent)
{
    bool bFinishEdit = true;

    switch (iButtonId) {
    case 0:
        if(eEvent == beReleased) {
            m_uiDigitSelect = (m_uiDigitSelect+1)%3;
            // Make pairs (6,7) (8,8) (9,9)
            m_pModel->SetEditingDigit(m_uiDigitSelect==0 ? 6 : 7+m_uiDigitSelect, 7+m_uiDigitSelect);
            bFinishEdit = false;
        }
        break;
    case 2:
        // TODO SET/RST
        break;
    case 3:
        // TODO FT/ET
        break;
    case 4:
        // TODO <->/FREQ
        if(eEvent == beReleased && m_pModel) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iAdfId == 1 ? xplane::Commands::cmAdf1Flip : xplane::Commands::cmAdf2Flip));
                m_uiDigitSelect = 0;
            } else {
                uint32_t uiTmp = m_pModel->GetActiveValue();
                m_pModel->SetActiveValue(m_pModel->GetStandbyValue());
                m_pModel->SetStandbyValue(uiTmp);
                //m_pModel->SaveSettings();
            }
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

    // Finish value edit mode in case of any button is pressed except rotary knob.
    if(bFinishEdit) {
        m_pModel->SetEditingFinished();
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonEventXPNDR(int8_t iButtonId, ButtonEvents eEvent)
{
    bool bFinishEdit = true;

    switch (iButtonId) {
    case 0: // Go to next digit
        if(eEvent == beReleased && m_pModel && m_pModel->GetXpndrMode() != xmOff) {
            m_uiDigitSelect = (m_uiDigitSelect+1)%4;
            m_pModel->SetEditingDigit(6+m_uiDigitSelect);
            bFinishEdit = false;
        }
        break;
    case 1: // Ident
        if(eEvent == beReleased && m_pModel && m_pModel->GetXpndrMode() != xmOff) {
            m_pModel->SetXpndrIdent();
        }
        break;
    case 2: // ALT
        if(eEvent == beReleased && m_pModel) {
            m_pModel->SetXpndrMode(xmAlt);
        }
        break;
    case 3:// ON
        if(eEvent == beReleased && m_pModel) {
            m_pModel->SetXpndrMode(xmOn);
        }
        break;
    case 4: // SBY
        if(eEvent == beReleased && m_pModel) {
            m_pModel->SetXpndrMode(xmSby);
        }
        break;
    case 5: // OFF
        if(eEvent == bePressed && m_pModel) {
            m_pModel->SetXpndrShutdown(true);
        } else if(eEvent == beReleased && m_pModel) {
            m_pModel->SetXpndrShutdown(false);
        } else if(eEvent == beLongPress && m_pModel) {
            m_pModel->SetXpndrMode(xmOff);
        }
        break;
    case 6: // VFR (Set squawk 2000)
        if(eEvent == beReleased && m_pModel && m_pModel->GetXpndrMode() != xmOff) {
            m_pModel->SetActiveValue(2000);
        }
        break;
    default:
        break;
    }

    // Finish value edit mode in case of any button is pressed except rotary knob.
    if(bFinishEdit) {
        m_pModel->SetEditingFinished();
    }
}

// -------------------------------------------------------------------------
