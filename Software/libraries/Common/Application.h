#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdint.h>
#include <stddef.h>
#include <Arduino.h>

class Application
{
public:
        //! Constructor
        Application();

        //! Destructor.
        ~Application() {;}

        //! Initialize Application object - called from main (inheritance!!!).
        virtual void Initialize();

        //! Do the last moment initializations then enter the main loop.
        virtual void Run();

protected:
        //! Initializes settings, model etc.
        virtual void InitializeObjects();

        //! Initializes communication objects
        virtual void InitializeComm();

        //! Initializes input/output pins
        virtual void InitializeIO();

        //! Process anything in inherited classes.
        virtual void ProcessLoop(void);

        //! Process for 1 Hz
        virtual void Process1Hz() = 0;

        //! Process for 10Hz
        virtual void Process10Hz() = 0;

        //! Process for 100 Hz
        virtual void Process100Hz() = 0;

        //! Process for 1000 Hz
        virtual void Process1000Hz() = 0;

        //! Sets Loop count to some mode. Reimplement in inherited class.
        virtual void SetLoopCount(uint32_t uiCount) = 0;

private:
        //! Hold previous valid time (1Hz)
        uint32_t m_ui1HzTimeout;
        //! Hold previous valid time (10Hz)
        uint32_t m_ui10HzTimeout;
        //! Hold previous valid time (100Hz)
        uint32_t m_ui100HzTimeout;
        //! Hold previous valid time (1000Hz)
        uint32_t m_ui1000HzTimeout;

        //! Process loop counter.
        uint32_t m_uiLoopCnt;
};

#endif // APPLICATION_H
