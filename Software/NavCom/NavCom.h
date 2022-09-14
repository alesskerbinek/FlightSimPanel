#ifndef FLIGHTSIM_H
#define FLIGHTSIM_H

#include <stdint.h>
#include <stddef.h>
#include <Arduino.h>
#include <Application.h>
#include "SettingsNavCom.h"
#include "ModelNavCom.h"
#include "NetworkNavCom.h"
#include "ConfigPortNavCom.h"
#include "Output.h"
#include "Input.h"

class NavCom : public Application
{

public:
        //! Constructor
        NavCom();

        //! Destructor.
        ~NavCom() {;}

protected:
        //! Initializes settings, model etc.
        virtual void InitializeObjects() override;

        //! Initializes input/output pins
        virtual void InitializeIO() override;

        //! Process anything in inherited classes.
        virtual void ProcessLoop(void) override;

        //! Process for 1 Hz
        virtual void Process1Hz() override;

        //! Process for 10Hz
        virtual void Process10Hz() override;

        //! Process for 100 Hz
        virtual void Process100Hz() override;

        //! Process for 1000 Hz
        virtual void Process1000Hz() override;

        //! Sets Loop count to model.
        virtual void SetLoopCount(uint32_t uiCount);

protected:
        // ------- Object pointers -------

        //! WiFi communication manager
        NetworkNavCom* m_pNetwork;

        //! Pointer to main settings. Make sure you read values from Flash before you use them.
        SettingsNavCom* m_pSet;

        //! Production mode object
        ConfigPortNavCom* m_pConfig;

        //! Model pointer;
        ModelNavCom* m_pModel;

        //! Output pointer;
        Output* m_pOutput;

        //! Input pointer;
        Input* m_pInput;

        // ------- App variables -------

};

#endif // FLIGHTSIM_H
