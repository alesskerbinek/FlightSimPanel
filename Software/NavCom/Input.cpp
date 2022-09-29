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
    // TODO check rotaries, buttons and switches

//    switch (m_pModel->GetUnitType()) {
//    case utCOM:     ProcessCOM();   break;
//    case utVOR:
    ProcessVOR();   //break;
//    case utADF:     ProcessADF();   break;
//    case utXPNDR:   ProcessXPNDR(); break;
//    default: break;
//    }
}

// -------------------------------------------------------------------------

void Input::ProcessVOR()
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
                if(m_auiButtonValues[ui] > 50) {
                    HandleButtonRelease(ui);
                }
                m_auiButtonValues[ui] = 0;
            }
        }
    }

    //Serial.printf("Analog: %03i, %03i\n", analogRead(ROT2_A), analogRead(ROT2_B));
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
    // TODO switch unit type

    if(iId == 0) {
        if(m_pModel) {
            if(m_bIsMHz) {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, iDir > 0 ? xplane::Commands::cmNav1StandbyCoarseUp : xplane::Commands::cmNav1StandbyCoarseDown));
                //m_pModel->IncrStandbyValue(iDir*1000);
            } else {
                m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, iDir > 0 ? xplane::Commands::cmNav1StandbyFineUp : xplane::Commands::cmNav1StandbyFineDown));
                //m_pModel->IncrStandbyValue(iDir*5);
            }
        }
    }
}

// -------------------------------------------------------------------------

void Input::HandleButtonRelease(int8_t iId)
{
    // TODO switch unit type

    /*
    AddToTxQueue(UdpDatagram(UdpDataType::dtDREF, DataRefs::drCom1Freq, (uint32_t)0x462AA001));
    AddToTxQueue(UdpDatagram(UdpDataType::dtCHAR, 0, (uint32_t)0x41));
    AddToTxQueue(UdpDatagram(UdpDataType::dtCMND, Commands::cmLandingGear));
    */

    switch (iId) {
    case 0:
        m_bIsMHz = !m_bIsMHz;
        break;
    case 1:
        break;
    case 2:
        if(m_pModel) {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCHAR, 0, (uint32_t)0x70));
        }
        break;
    case 3:
        if(m_pModel) {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmNav1Flip));
            //m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCMND, xplane::Commands::cmFlapsUp));

            //uint32_t uiTmp = m_pModel->GetActiveValue();
            //m_pModel->SetActiveValue(m_pModel->GetStandbyValue());
            //m_pModel->SetStandbyValue(uiTmp);
            //m_pModel->SaveSettings();
            m_bIsMHz = false;
        }
        break;
    case 4:
        if(m_pModel) {
            m_pModel->AddToTxQueue(xplane::UdpDatagram(xplane::UdpDataType::dtCHAR, 0, (uint32_t)0x50));
        }
        break;
    case 5:
        break;
    case 6:
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------
