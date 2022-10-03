#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdint.h>
#include <stddef.h>
#include "ApplicationDefines.h"
#include "ModelNavCom.h"

class Output
{
public:
    //! Public constructor.
    Output();

    //! Public destructor.
    ~Output(){;}

    //! Initializes class.
    void Initialize();

    //! Main state machine loop.
    //! Handle 7 segment displays and LEDs.
    void Process();

    //! Updates values on display from model.
    virtual void UpdateValues();

private:
    //! Selects digit to set value.
    void SelectDigit(int8_t iDigit);

    //! Sets value to currently selected digit.
    void SetSegments(int8_t iSegments);

    //! Sets active VHF frequency
    void SetActiveMHz(uint32_t uiVal, bool b833);

    //! Sets standby VHF frequency
    void SetStandbyMHz(uint32_t uiVal, bool b833);

    //! Sets active MF/HF frequency
    void SetActiveKHz(uint32_t uiVal);

    //! Sets standby MF/HF frequency
    void SetStandbyKHz(uint32_t uiVal);

    //! Sets transponder squawk
    void SetSquawk(uint32_t uiVal);

    //! Sets transponder mode
    void SetMode(uint32_t uiVal);

private:
    //! Model pointer;
    ModelNavCom* m_pModel = nullptr;

    //! Selected digit counter
    int16_t m_iDigit = 0;

    //! Current digit values buffer
    uint8_t m_auiDigitValues[DIGIT_COUNT];

    //! Digit addresses
    uint8_t m_auiDigitAddresses[DIGIT_COUNT] = {DIG_1, DIG_2, DIG_3, DIG_4, DIG_5, DIG_6, DIG_7, DIG_8, DIG_9, DIG_10, DIG_11, DIG_12};

    //! Number characters
    uint8_t m_auiChars[10] = {CH_0, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9};
};

#endif // OUTPUT_H
