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
    if(m_pModel && m_pModel->GetUnitMode() != gmOff && iRotaryId == 0) // Right rotary - Frequency
    {
        // Select first digit if not in edit mode already
        if(m_pModel->IsEditingDigit() == false) {
            m_uiDigitSelect = 0;
        }
        if(m_uiDigitSelect == 0) {
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
        m_pModel->SetEditingDigit(6+m_uiDigitSelect*3,8+m_uiDigitSelect*3);
    }
    else if (m_pModel && m_pModel->GetUnitMode() != gmOff && iRotaryId == 1) // Left rotary - Volume
    {
        // This is just for display. No action is performed on simulator.
        m_pModel->SetVolume(m_pModel->GetVolume()+iDirection*5);
        m_pModel->SetEditingDigit(9,11);
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollVOR(int8_t iRotaryId, int8_t iDirection, int8_t iVorId)
{
    if(m_pModel && m_pModel->GetUnitMode() != gmOff && iRotaryId == 0) // Right rotary - Frequency
    {
        // Select first digit if not in edit mode already
        if(m_pModel->IsEditingDigit() == false) {
            m_uiDigitSelect = 0;
        }
        if(m_uiDigitSelect == 0) {
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                if(iVorId == 1) {
                    cmd = iDirection > 0 ? xplane::Commands::cmNav1StandbyCoarseUp : xplane::Commands::cmNav1StandbyCoarseDown;
                } else {
                    cmd = iDirection > 0 ? xplane::Commands::cmNav2StandbyCoarseUp : xplane::Commands::cmNav2StandbyCoarseDown;
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
                    cmd = iDirection > 0 ? xplane::Commands::cmNav2StandbyFineUp : xplane::Commands::cmNav2StandbyFineDown;
                }
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            } else {
                m_pModel->IncrStandbyValue(iDirection*50, 1000);
            }
        }
        // Switch between (9,11) and (6,8)
        m_pModel->SetEditingDigit(6+m_uiDigitSelect*3,8+m_uiDigitSelect*3);
    }
    else if (m_pModel && m_pModel->GetUnitMode() != gmOff && iRotaryId == 1) // Left rotary - OBS
    {
        if(m_pModel->IsHDGCommand()) {
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                cmd = iDirection > 0 ? xplane::Commands::cmAutopilotHdgUp : xplane::Commands::cmAutopilotHdgDown;
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
                m_pModel->SetHDG(m_pModel->GetHDG()); // just set the same value to reset the timer
            } else {
                m_pModel->SetHDG(m_pModel->GetHDG()+iDirection);
            }
        } else {
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                if(iVorId == 1) {
                    cmd = iDirection > 0 ? xplane::Commands::cmNav1ObsUp : xplane::Commands::cmNav1ObsDown;
                } else {
                    cmd = iDirection > 0 ? xplane::Commands::cmNav2ObsUp : xplane::Commands::cmNav2ObsDown;
                }
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
                m_pModel->SetOBS(m_pModel->GetOBS()); // just set the same value to reset the timer
            } else {
                m_pModel->SetOBS(m_pModel->GetOBS()+iDirection);
            }
        }
        m_pModel->SetEditingDigit(9,11);
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollADF(int8_t /*iRotaryId*/, int8_t iDirection, int8_t iAdfId)
{
    if(m_pModel && m_pModel->GetUnitMode() != gmOff)
    {
        // Select first digit if not in edit mode already
        if(m_pModel->IsEditingDigit() == false) {
            m_uiDigitSelect = 0;
        }
        if(m_uiDigitSelect == 0) { // Change XX00
            if(m_pModel->GetUnitMode() == amFrq) {
                if(m_pModel->IsSimConnected()) {
                    xplane::Commands cmd;
                    if(iAdfId == 1) {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf1StandbyXX00Up : xplane::Commands::cmAdf1StandbyXX00Down;
                    } else {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf2StandbyXX00Up : xplane::Commands::cmAdf2StandbyXX00Down;
                    }
                    m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
                } else {
                    m_pModel->IncrStandbyValue(iDirection*100, 0);
                }
                m_pModel->SetEditingDigit(6,7);
            } else if(m_pModel->GetUnitMode() == amElT || m_pModel->GetUnitMode() == amFlT) {
                if(m_pModel->IsSimConnected()) {
                    xplane::Commands cmd;
                    if(iAdfId == 1) {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf1ActiveXX00Up : xplane::Commands::cmAdf1ActiveXX00Down;
                    } else {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf2ActiveXX00Up : xplane::Commands::cmAdf2ActiveXX00Down;
                    }
                    m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
                } else {
                    m_pModel->IncrActiveValue(iDirection*100, 0);
                }
                m_pModel->SetEditingDigit(0,1);
            }
        } else if(m_uiDigitSelect == 1) { // Change 00X0
            if(m_pModel->GetUnitMode() == amFrq) {
                if(m_pModel->IsSimConnected()) {
                    xplane::Commands cmd;
                    if(iAdfId == 1) {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf1Standby00X0Up : xplane::Commands::cmAdf1Standby00X0Down;
                    } else {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf2Standby00X0Up : xplane::Commands::cmAdf2Standby00X0Down;
                    }
                    m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
                } else {
                    m_pModel->IncrStandbyValue(iDirection*10, 100);
                }
                m_pModel->SetEditingDigit(8);
            } else if(m_pModel->GetUnitMode() == amElT || m_pModel->GetUnitMode() == amFlT) {
                if(m_pModel->IsSimConnected()) {
                    xplane::Commands cmd;
                    if(iAdfId == 1) {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf1Active00X0Up : xplane::Commands::cmAdf1Active00X0Down;
                    } else {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf2Active00X0Up : xplane::Commands::cmAdf2Active00X0Down;
                    }
                    m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
                } else {
                    m_pModel->IncrActiveValue(iDirection*10, 100);
                }
                m_pModel->SetEditingDigit(2);
            }
        } else { // Change 000X
            if(m_pModel->GetUnitMode() == amFrq) {
                if(m_pModel->IsSimConnected()) {
                    xplane::Commands cmd;
                    if(iAdfId == 1) {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf1Standby000XUp : xplane::Commands::cmAdf1Standby000XDown;
                    } else {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf2Standby000XUp : xplane::Commands::cmAdf2Standby000XDown;
                    }
                    m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
                } else {
                    m_pModel->IncrStandbyValue(iDirection, 10);
                }
                m_pModel->SetEditingDigit(9);
            } else if(m_pModel->GetUnitMode() == amElT || m_pModel->GetUnitMode() == amFlT) {
                if(m_pModel->IsSimConnected()) {
                    xplane::Commands cmd;
                    if(iAdfId == 1) {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf1Active000XUp : xplane::Commands::cmAdf1Active000XDown;
                    } else {
                        cmd = iDirection > 0 ? xplane::Commands::cmAdf2Active000XUp : xplane::Commands::cmAdf2Active000XDown;
                    }
                    m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
                } else {
                    m_pModel->IncrActiveValue(iDirection, 10);
                }
                m_pModel->SetEditingDigit(3);
            }
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleRotaryScrollXPNDR(int8_t /*iRotaryId*/, int8_t iDirection)
{
    if(m_pModel && m_pModel->GetUnitMode() != gmOff)
    {
        // Select first digit if not in edit mode already
        if(m_pModel->IsEditingDigit() == false) {
            m_uiDigitSelect = 0;
        }
        if(m_uiDigitSelect == 0) { // Change X000
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                cmd = iDirection > 0 ? xplane::Commands::cmXpdrX000Up : xplane::Commands::cmXpdrX000Down;
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            } else {
                m_pModel->IncrActiveValue(iDirection*1000, 8000);
            }
            m_pModel->SetEditingDigit(6);
        } else if(m_uiDigitSelect == 1) { // Change 0X00
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                cmd = iDirection > 0 ? xplane::Commands::cmXpdr0X00Up : xplane::Commands::cmXpdr0X00Down;
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            } else {
                m_pModel->IncrActiveValue(iDirection*100, 800);
            }
            m_pModel->SetEditingDigit(7);
        } else if(m_uiDigitSelect == 2) { // Change 00X0
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                cmd = iDirection > 0 ? xplane::Commands::cmXpdr00X0Up : xplane::Commands::cmXpdr00X0Down;
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
            } else {
                m_pModel->IncrActiveValue(iDirection*10, 80);
            }
            m_pModel->SetEditingDigit(8);
        } else { // Change 000X
            if(m_pModel->IsSimConnected()) {
                xplane::Commands cmd;
                cmd = iDirection > 0 ? xplane::Commands::cmXpdr000XUp : xplane::Commands::cmXpdr000XDown;
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, cmd));
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
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsEditingDigit() == true) {
                m_uiDigitSelect = (m_uiDigitSelect+1)%2;
            } else {
                m_uiDigitSelect = 0;
            }
            // Switch between (9,11) and (6,8)
            m_pModel->SetEditingDigit(6+m_uiDigitSelect*3,8+m_uiDigitSelect*3);
        } // Turn device on/off
        else if(eEvent == beLongPress && m_pModel) {
            if(m_pModel->GetUnitMode() != gmOff) {
                m_pModel->SetUnitMode(gmOff);
                m_pModel->SaveSettings();
            } else {
                m_pModel->SetUnitMode(amFrq);
            }
        }
        break;
    case 3: // Flip
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iComId == 1 ? xplane::Commands::cmCom1Flip : xplane::Commands::cmCom2Flip));
                m_uiDigitSelect = 0;
            } else {
                uint32_t uiTmp = m_pModel->GetActiveValue();
                m_pModel->SetActiveValue(m_pModel->GetStandbyValue());
                m_pModel->SetStandbyValue(uiTmp);
            }
            m_pModel->SetEditingFinished();
        }
        break;
    case 1: // Set Emergency frequency
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtDREF,
                        iComId == 1 ? xplane::DataRefs::drCom1ActiveFreq : xplane::DataRefs::drCom2ActiveFreq,
                        (uint32_t)0x47ed4e00)); // == 121500.0f ==> 121.500
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
    switch (iButtonId) {
    case 0:
        // Select next digit
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsEditingDigit() == true) {
                m_uiDigitSelect = (m_uiDigitSelect+1)%2;
            } else {
                m_uiDigitSelect = 0;
            }
            // Switch between (9,11) and (6,8)
            m_pModel->SetEditingDigit(6+m_uiDigitSelect*3,8+m_uiDigitSelect*3);
        } // Turn device on/off
        else if(eEvent == beLongPress && m_pModel) {
            if(m_pModel->GetUnitMode() != gmOff) {
                m_pModel->SetUnitMode(gmOff);
                m_pModel->SaveSettings();
            } else {
                m_pModel->SetUnitMode(amFrq);
            }
        }
        break;
    case 1: // OBS/Bug
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsHDGCommand()) {
                m_pModel->SetHDGFinished();
                m_pModel->SetEditingFinished();
            } else if(m_pModel->IsOBSCommand()) {
                m_pModel->SetOBSFinished();
                m_pModel->SetHDG(m_pModel->GetHDG()); // just set the same value to reset the timer
                m_pModel->SetEditingDigit(9,11);
            } else {
                m_pModel->SetOBS(m_pModel->GetOBS()); // just set the same value to reset the timer
                m_pModel->SetEditingDigit(9,11);
            }
        }
        break;
    case 3: // Flip
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iVorId == 1 ? xplane::Commands::cmNav1Flip : xplane::Commands::cmNav2Flip));
                m_uiDigitSelect = 0;
            } else {
                uint32_t uiTmp = m_pModel->GetActiveValue();
                m_pModel->SetActiveValue(m_pModel->GetStandbyValue());
                m_pModel->SetStandbyValue(uiTmp);
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
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsEditingDigit() == true) {
                m_uiDigitSelect = (m_uiDigitSelect+1)%3;
            } else {
                m_uiDigitSelect = 0;
            }
            if(m_pModel->GetUnitMode() == amFrq) {
                // Make pairs (6,7) (8,8) (9,9)
                m_pModel->SetEditingDigit(m_uiDigitSelect==0 ? 6 : 7+m_uiDigitSelect, 7+m_uiDigitSelect);
            } else if(m_pModel->GetUnitMode() == amElT || m_pModel->GetUnitMode() == amFlT) {
                // Make pairs (0,1) (2,2) (3,3)
                m_pModel->SetEditingDigit(m_uiDigitSelect==0 ? 0 : 1+m_uiDigitSelect, 1+m_uiDigitSelect);
            }
        } else if(eEvent == beLongPress && m_pModel) {
            if(m_pModel->GetUnitMode() != gmOff) {
                m_pModel->SetUnitMode(gmOff);
                m_pModel->SaveSettings();
            } else {
                m_pModel->SetUnitMode(amFrq);
                // Reset timers on unit turn on
                m_pModel->ResetET();
                m_pModel->ResetFT();
            }
        }
        bFinishEdit = false;
        break;
    case 2: // SET/RST
        // KR 87 Manual: pressing the SET/RST button will reset the elapsed timer whether it is being
        // displayed or not.
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            m_pModel->ResetET();
        }
        break;
    case 3: // FT/ET
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->GetUnitMode() == amFlT) {
                m_pModel->SetUnitMode(amElT);
            } else {
                m_pModel->SetUnitMode(amFlT);
            }
        }
        break;
    case 4:
        // <->/FREQ
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->GetUnitMode() == amFrq) {
                // Switch frequencies
                if(m_pModel->IsSimConnected()) {
                    m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                                iAdfId == 1 ? xplane::Commands::cmAdf1Flip : xplane::Commands::cmAdf2Flip));
                    m_uiDigitSelect = 0;
                } else {
                    uint32_t uiTmp = m_pModel->GetActiveValue();
                    m_pModel->SetActiveValue(m_pModel->GetStandbyValue());
                    m_pModel->SetStandbyValue(uiTmp);
                }
            } else {
                m_pModel->SetUnitMode(amFrq);
            }
        }
        break;
    case 5:
        // BFO ON/OFF
        if(m_pModel->IsSimConnected()) {
            if(eEvent == beReleased && m_pModel) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iAdfId == 1 ? xplane::Commands::cmAdf1PowerOn : xplane::Commands::cmAdf2PowerOn));
            } else if(eEvent == bePressed && m_pModel) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iAdfId == 1 ? xplane::Commands::cmAdf1PowerTone : xplane::Commands::cmAdf2PowerTone));
            }
        }
        // Nothing to do in demo mode
        break;
    case 6:
        // ADF/ANT
        if(m_pModel->IsSimConnected()) {
            if(eEvent == beReleased && m_pModel) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iAdfId == 1 ? xplane::Commands::cmAdf1PowerAnt : xplane::Commands::cmAdf2PowerAnt));
            } else if(eEvent == bePressed && m_pModel) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND,
                            iAdfId == 1 ? xplane::Commands::cmAdf1PowerOn : xplane::Commands::cmAdf2PowerOn));
            }
        }
        // Nothing to do in demo mode
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
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsEditingDigit() == true) {
                m_uiDigitSelect = (m_uiDigitSelect+1)%4;
            } else {
                m_uiDigitSelect = 0;
            }
            m_pModel->SetEditingDigit(6+m_uiDigitSelect);
        }
        bFinishEdit = false;
        break;
    case 1: // OFF
        if(eEvent == bePressed && m_pModel) {
            m_pModel->SetXpndrShutdown(true);
        } else if(eEvent == beReleased && m_pModel) {
            m_pModel->SetXpndrShutdown(false);
        } else if(eEvent == beLongPress && m_pModel) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmXpdrOff));
            } else {
                m_pModel->SetUnitMode(gmOff);
                m_pModel->SaveSettings();
            }
        }
        break;
    case 2: // ALT
        if(eEvent == beReleased && m_pModel) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmXpdrAlt));
            } else {
                m_pModel->SetUnitMode(xmAlt);
            }
        }
        break;
    case 3:// ON
        if(eEvent == beReleased && m_pModel) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmXpdrOn));
            } else {
                m_pModel->SetUnitMode(gmOn);
            }
        }
        break;
    case 4: // SBY
        if(eEvent == beReleased && m_pModel) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmXpdrSby));
            } else {
                m_pModel->SetUnitMode(xmSby);
            }
        }
        break;
    case 5: // Ident
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmXpdrIdent));
            } else {
                m_pModel->SetXpndrIdent(true);
            }
        }
        break;
    case 6: // VFR (Set squawk 2000)
        if(eEvent == beReleased && m_pModel && m_pModel->GetUnitMode() != gmOff) {
            if(m_pModel->IsSimConnected()) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtDREF,
                        xplane::DataRefs::drXpdrCode,
                        (uint32_t)0x44fa0000)); // == 2000.0
            } else {
                m_pModel->SetActiveValue(2000);
            }
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
