#include <Arduino.h>
#include <Helper.h>
#include "ApplicationDefines.h"
#include "Output.h"

// -------------------------------------------------------------------------

Output::Output()
{
    memset(&m_auiDigitValues, CH_SPACE, sizeof(m_auiDigitValues));
}

// -------------------------------------------------------------------------

void Output::Initialize()
{
    m_pModel = ModelNavCom::GetInstance();
}

// -------------------------------------------------------------------------

void Output::Process()
{
    //Select the digit and set value on the bus
    SelectDigit(m_auiDigitAddresses[m_iDigit]);
    SetSegments(m_auiDigitValues[m_iDigit]);

    // Go to next digit
    m_iDigit++;
    m_iDigit %= DIGIT_COUNT;
}

// -------------------------------------------------------------------------

void Output::SelectDigit(int8_t iDigit)
{
  digitalWrite(DIG_SEL_A, iDigit & 0x01);
  digitalWrite(DIG_SEL_B, iDigit & 0x02);
  digitalWrite(DIG_SEL_C, iDigit & 0x04);
  digitalWrite(DIG_SEL_D, iDigit & 0x08);
}

// -------------------------------------------------------------------------

void Output::SetSegments(int8_t iSegments)
{
  digitalWrite(SEG_A, iSegments & 0x01);
  digitalWrite(SEG_B, iSegments & 0x02);
  digitalWrite(SEG_C, iSegments & 0x04);
  digitalWrite(SEG_D, iSegments & 0x08);
  digitalWrite(SEG_E, iSegments & 0x10);
  digitalWrite(SEG_F, iSegments & 0x20);
  digitalWrite(SEG_G, iSegments & 0x40);
  digitalWrite(SEG_DP, iSegments & 0x80);
}

// -------------------------------------------------------------------------

void Output::UpdateValues()
{
    if(m_pModel)
    {
        // Set Connection status LED. We turn LED on with HIGH.
        digitalWrite(PIN_CONN, m_pModel->IsSimConnected() ? LOW : HIGH);

        // Unit specific output
        switch (m_pModel->GetUnitType()) {
        case utCom1:
        case utCom2:
            SetActiveMHz(m_pModel->GetActiveValue(), true);
            if(m_pModel->IsVolumeCommand()) {
                SetVolume(m_pModel->GetVolume());
            } else {
                SetStandbyMHz(m_pModel->GetStandbyValue(), true);
            }
            break;
        case utVOR1:
        case utVOR2:
            SetActiveMHz(m_pModel->GetActiveValue(), false);
            SetStandbyMHz(m_pModel->GetStandbyValue(), false);
            break;
        case utADF1:
        case utADF2:
            SetActiveKHz(m_pModel->GetActiveValue());
            SetStandbyKHz(m_pModel->GetStandbyValue());
            // TODO or SetTime(m_pModel->GetFlightTime());
            // TODO or SetTime(m_pModel->GetElapsedTime());
            break;
        case utXPNDR:
            if(m_pModel->GetXpndrMode() != xmOff) {
                SetSquawk(m_pModel->GetActiveValue());
                if(m_pModel->IsXpndrIdent()) {
                    SetIdent();
                } else if(m_pModel->IsXpndrShutdown()) {
                    SetShutdown();
                } else {
                    SetMode(m_pModel->GetXpndrMode());
                }
            } else {
                SetDisplayOff();
            }
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------

void Output::SetActiveMHz(uint32_t uiVal, bool b833)
{
    if(DIGIT_COUNT >= 6)
    {
        if(b833) {
            m_auiDigitValues[5] = m_auiChars[uiVal % 10];
            m_auiDigitValues[4] = m_auiChars[uiVal/10 % 10];
            m_auiDigitValues[3] = m_auiChars[uiVal/100 % 10];
            m_auiDigitValues[2] = m_auiChars[uiVal/1000 % 10] & DECIMAL_POINT;
            m_auiDigitValues[1] = m_auiChars[uiVal/10000 % 10];
            m_auiDigitValues[0] = m_auiChars[uiVal/100000 % 10];
        } else {
            m_auiDigitValues[5] = m_auiChars[uiVal/10 % 10];
            m_auiDigitValues[4] = m_auiChars[uiVal/100 % 10];
            m_auiDigitValues[3] = m_auiChars[uiVal/1000 % 10] & DECIMAL_POINT;
            m_auiDigitValues[2] = m_auiChars[uiVal/10000 % 10];
            m_auiDigitValues[1] = m_auiChars[uiVal/100000 % 10];
            m_auiDigitValues[0] = CH_SPACE;
        }
    }
}

// -------------------------------------------------------------------------

void Output::SetStandbyMHz(uint32_t uiVal, bool b833)
{
    if(DIGIT_COUNT >= 12)
    {
        if(b833) {
            m_auiDigitValues[11] = m_auiChars[uiVal % 10];
            m_auiDigitValues[10] = m_auiChars[uiVal/10 % 10];
            m_auiDigitValues[9]  = m_auiChars[uiVal/100 % 10];
            m_auiDigitValues[8]  = m_auiChars[uiVal/1000 % 10] & DECIMAL_POINT;
            m_auiDigitValues[7]  = m_auiChars[uiVal/10000 % 10];
            m_auiDigitValues[6]  = m_auiChars[uiVal/100000 % 10];
        } else {
            m_auiDigitValues[11] = CH_SPACE;
            m_auiDigitValues[10] = m_auiChars[uiVal/10 % 10];
            m_auiDigitValues[9] = m_auiChars[uiVal/100 % 10];
            m_auiDigitValues[8] = m_auiChars[uiVal/1000 % 10] & DECIMAL_POINT;
            m_auiDigitValues[7] = m_auiChars[uiVal/10000 % 10];
            m_auiDigitValues[6] = m_auiChars[uiVal/100000 % 10];
        }
    }
}

// -------------------------------------------------------------------------

void Output::SetActiveKHz(uint32_t uiVal)
{
    if(DIGIT_COUNT >= 4)
    {
        m_auiDigitValues[3] = m_auiChars[uiVal % 10];
        m_auiDigitValues[2] = m_auiChars[uiVal/10 % 10];
        m_auiDigitValues[1] = m_auiChars[uiVal/100 % 10];
        m_auiDigitValues[0] = uiVal >= 1000 ? m_auiChars[uiVal/1000 % 10] : CH_SPACE;
    }
}

// -------------------------------------------------------------------------

void Output::SetStandbyKHz(uint32_t uiVal)
{
    if(DIGIT_COUNT >= 10)
    {
        m_auiDigitValues[9]  = m_auiChars[uiVal % 10];
        m_auiDigitValues[8]  = m_auiChars[uiVal/10 % 10];
        m_auiDigitValues[7]  = m_auiChars[uiVal/100 % 10];
        m_auiDigitValues[6]  = uiVal >= 1000 ? m_auiChars[uiVal/1000 % 10] : CH_SPACE;
    }
}

// -------------------------------------------------------------------------

void Output::SetSquawk(uint32_t uiVal)
{
    if(DIGIT_COUNT >= 10)
    {
        m_auiDigitValues[9]  = m_auiChars[uiVal % 10];
        m_auiDigitValues[8]  = m_auiChars[uiVal/10 % 10];
        m_auiDigitValues[7]  = m_auiChars[uiVal/100 % 10];
        m_auiDigitValues[6]  = m_auiChars[uiVal/1000 % 10];
    }
}

// -------------------------------------------------------------------------

void Output::SetMode(XpndrModes eMode)
{
    if(DIGIT_COUNT >= 3)
    {
        switch (eMode) {
        case xmSby:
            m_auiDigitValues[0] = CH_S;
            m_auiDigitValues[1] = CH_B;
            m_auiDigitValues[2] = CH_Y;
            break;
        case xmOn:
            m_auiDigitValues[0] = CH_O;
            m_auiDigitValues[1] = CH_N;
            m_auiDigitValues[2] = CH_SPACE;
            break;
        case xmAlt:
            m_auiDigitValues[0] = CH_A;
            m_auiDigitValues[1] = CH_L;
            m_auiDigitValues[2] = CH_T;
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------

void Output::SetIdent()
{
    if(DIGIT_COUNT >= 3)
    {
        // Blink 1Hz with 50% duty cycle
        if(helper::GetTime() % 1000 < 500) {
            m_auiDigitValues[0] = CH_I;
            m_auiDigitValues[1] = CH_D;
            m_auiDigitValues[2] = CH_T;
        } else {
            m_auiDigitValues[0] = CH_SPACE;
            m_auiDigitValues[1] = CH_SPACE;
            m_auiDigitValues[2] = CH_SPACE;
        }
    }
}

// -------------------------------------------------------------------------

void Output::SetShutdown()
{
    if(DIGIT_COUNT >= 3)
    {
        m_auiDigitValues[0] = CH_O;
        m_auiDigitValues[1] = CH_F;
        m_auiDigitValues[2] = CH_F;
    }
}

// -------------------------------------------------------------------------

void Output::SetDisplayOff()
{
    for(int i=0; i<DIGIT_COUNT; i++) {
        m_auiDigitValues[i] = CH_SPACE;
    }
}

// -------------------------------------------------------------------------

void Output::SetVolume(uint8_t uiVol)
{
    if(DIGIT_COUNT >= 12)
    {
        m_auiDigitValues[11] = m_auiChars[uiVol % 10];
        m_auiDigitValues[10] = uiVol >= 10 ? m_auiChars[uiVol/10 % 10] : CH_SPACE;
        m_auiDigitValues[9]  = uiVol >= 100 ? m_auiChars[uiVol/100 % 10] : CH_SPACE;
        m_auiDigitValues[8]  = CH_L;
        m_auiDigitValues[7]  = CH_O;
        m_auiDigitValues[6]  = CH_V;
    }
}

// -------------------------------------------------------------------------
