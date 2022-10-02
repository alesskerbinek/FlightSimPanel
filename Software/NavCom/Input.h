#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stddef.h>
#include "ApplicationDefines.h"
#include "ModelNavCom.h"

class Input
{
public:
    //! Public constructor.
    Input();

    //! Public destructor.
    ~Input(){;}

    //! Initializes class.
    void Initialize();

    //! Main state machine loop.
    void Process();

    //! Returns given rotary counter
    int16_t GetRotary(uint8_t uiId);

private:
    //! Check rotary encoders states
    void CheckRotaries();

    //! Check button states
    void CheckButtons();

    //! Handles rotary encoder change
    void HandleRotaryScroll(int8_t iRotaryId, int8_t iDirection);

    //! Handles rotary encoder change for Radio unit
    void HandleRotaryScrollCOM(int8_t iRotaryId, int8_t iDirection, int8_t iComId);

    //! Handles rotary encoder change for Nav unit
    void HandleRotaryScrollVOR(int8_t iRotaryId, int8_t iDirection, int8_t iVorId);

    //! Handles rotary encoder change for ADF unit
    void HandleRotaryScrollADF(int8_t iRotaryId, int8_t iDirection, int8_t iAdfId);

    //! Handles rotary encoder change for Transponder
    void HandleRotaryScrollXPNDR(int8_t iRotaryId, int8_t iDirection);

    //! Handles button press
    void HandleButtonRelease(int8_t iButtonId);

    //! Handles button press for Radio unit
    void HandleButtonReleaseCOM(int8_t iButtonId, int8_t iComId);

    //! Handles button press for Nav unit
    void HandleButtonReleaseVOR(int8_t iButtonId, int8_t iVorId);

    //! Handles button press for ADF unit
    void HandleButtonReleaseADF(int8_t iButtonId, int8_t iAdfId);

    //! Handles button press for Transponder
    void HandleButtonReleaseXPNDR(int8_t iButtonId);

private:
    //! Model pointer;
    ModelNavCom* m_pModel = nullptr;

    //! Current button values
    uint16_t m_auiButtonValues[BUTTON_COUNT];

    //! Buttonaddresses
    uint8_t m_auiButtonAddresses[BUTTON_COUNT] = {PIN_SW1, PIN_SW2, PIN_SW3, PIN_SW4, PIN_SW5, ROT2_B, ROT2_A};

    //! Rotary last state
    bool m_abLastState[ROTARY_COUNT];

    //! Rotary change timestamp
    uint32_t m_auiLastChange[ROTARY_COUNT];

    //! Actually used buttons in current unit
    uint8_t m_uiUsedButtons = BUTTON_COUNT;

    //! Flag indicating whether we are in MHz or kHz editing mode.
    uint16_t m_uiDigitSelect = 0;
};

#endif // INPUT_H
