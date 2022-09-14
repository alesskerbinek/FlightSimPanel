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
    //! Main state machine for VOR unit
    void ProcessVOR();

    //! Check rotary encoders states
    void CheckRotaries();

    //! Check button states
    void CheckButtons();

    //! Handles rotary encoder change
    void HandleRotaryScroll(int8_t iId, int8_t iDir);

    //! Handles button press
    void HandleButtonRelease(int8_t iId);

private:
    //! Model pointer;
    ModelNavCom* m_pModel = nullptr;

    //! Current button values
    uint16_t m_auiButtonValues[BUTTON_COUNT];

    //! Buttonaddresses
    uint8_t m_auiButtonAddresses[BUTTON_COUNT] = {PIN_SW1, PIN_SW2, PIN_SW3, PIN_SW4, PIN_SW5, ROT2_B, ROT2_A};

    //! Rotary last state
    bool m_abLastState[ROTARY_COUNT];

    //! Actually used buttons in current unit
    uint8_t m_uiUsedButtons = BUTTON_COUNT;

    //! Flag indicating whether we are in MHz or kHz editing mode.
    bool m_bIsMHz = false;
};

#endif // INPUT_H
