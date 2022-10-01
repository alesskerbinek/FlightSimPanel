#include <Arduino.h>
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
    // Handle 7 segment displays and LEDs
    bool bDP = m_iDigit == 2 || m_iDigit == 8;

    //Select the digit and set value on the bus
    SelectDigit(m_auiDigitAddresses[m_iDigit]);
    SetSegments(m_auiDigitValues[m_iDigit], bDP);

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

void Output::SetSegments(int8_t iSegments, bool bDecimalPoint)
{
  digitalWrite(SEG_A, iSegments & 0x01);
  digitalWrite(SEG_B, iSegments & 0x02);
  digitalWrite(SEG_C, iSegments & 0x04);
  digitalWrite(SEG_D, iSegments & 0x08);
  digitalWrite(SEG_E, iSegments & 0x10);
  digitalWrite(SEG_F, iSegments & 0x20);
  digitalWrite(SEG_G, iSegments & 0x40);
  digitalWrite(SEG_DP, !bDecimalPoint);
}

// -------------------------------------------------------------------------

void Output::UpdateValues()
{
    if(m_pModel) {
        switch (m_pModel->GetUnitType()) {
        case utCom:
            SetActive(m_pModel->GetActiveValue(), true);
            SetStandby(m_pModel->GetStandbyValue(), true);
            break;
        case utVOR:
            SetActive(m_pModel->GetActiveValue(), false);
            SetStandby(m_pModel->GetStandbyValue(), false);
            break;
        case utADF:
            // TODO SetActiveADF
            // TODO SetStandbyADF or SetTimeADF
            break;
        case utXPNDR:
            // TODO SetSquawk
            // TODO SetMode
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------

void Output::SetActive(uint32_t uiVal, bool b833)
{    
    if(DIGIT_COUNT >= 6)
    {
        // TODO if(833)
        m_auiDigitValues[5] = m_auiChars[uiVal % 10];
        m_auiDigitValues[4] = m_auiChars[uiVal/10 % 10];
        m_auiDigitValues[3] = m_auiChars[uiVal/100 % 10];
        m_auiDigitValues[2] = m_auiChars[uiVal/1000 % 10];
        m_auiDigitValues[1] = m_auiChars[uiVal/10000 % 10];
        m_auiDigitValues[0] = m_auiChars[uiVal/100000 % 10];
    }
}

// -------------------------------------------------------------------------

void Output::SetStandby(uint32_t uiVal, bool b833)
{
    if(DIGIT_COUNT >= 12)
    {
        // TODO if(833)
        m_auiDigitValues[11] = m_auiChars[uiVal % 10];
        m_auiDigitValues[10] = m_auiChars[uiVal/10 % 10];
        m_auiDigitValues[9]  = m_auiChars[uiVal/100 % 10];
        m_auiDigitValues[8]  = m_auiChars[uiVal/1000 % 10];
        m_auiDigitValues[7]  = m_auiChars[uiVal/10000 % 10];
        m_auiDigitValues[6]  = m_auiChars[uiVal/100000 % 10];
    }
}

// -------------------------------------------------------------------------
