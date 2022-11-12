#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stddef.h>
#include "ApplicationDefines.h"
#include "ModelNavCom.h"

class Input
{
private:
    enum ButtonEvents {
        beUndefined,
        bePressed,
        beReleased,
        beLongPress,
    };

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
    void CheckRotaries(bool bLeft, bool bRight);

    //! Check button states
    void CheckButtons();

    //! Handles rotary encoder change
    void HandleRotaryScroll(int8_t iRotaryId, int8_t iDirection);

    //! Handles rotary encoder change for Radio unit
    void HandleRotaryScrollCOM(int8_t iRotaryId, int8_t iDirection, int8_t iComId);

    //! Handles rotary encoder change for Nav unit
    void HandleRotaryScrollVOR(int8_t iRotaryId, int8_t iDirection, int8_t iVorId);

    //! Handles rotary encoder change for ADF unit
    void HandleRotaryScrollADF(int8_t /*iRotaryId*/, int8_t iDirection, int8_t iAdfId);

    //! Handles rotary encoder change for Transponder
    void HandleRotaryScrollXPNDR(int8_t /*iRotaryId*/, int8_t iDirection);

    //! Handles button event. In case the button is pressed and hold down for more than
    //! 1000 cycles the events are declared in the following order: Pressed, LongPress, Released.
    //! Every Pressed event will be followed by Released.
    void HandleButtonEvent(int8_t iButtonId, ButtonEvents eEvent);

    //! Handles button event for Radio unit
    void HandleButtonEventCOM(int8_t iButtonId, int8_t iComId, ButtonEvents eEvent);

    //! Handles button event for Nav unit
    void HandleButtonEventVOR(int8_t iButtonId, int8_t iVorId, ButtonEvents eEvent);

    //! Handles button event for ADF unit
    void HandleButtonEventADF(int8_t iButtonId, int8_t iAdfId, ButtonEvents eEvent);

    //! Handles button event for Transponder
    void HandleButtonEventXPNDR(int8_t iButtonId, ButtonEvents eEvent);

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
