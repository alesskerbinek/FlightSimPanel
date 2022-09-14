#include "Helper.h"
#include "Application.h"

// -------------------------------------------------------------------------

Application::Application()
{
    m_ui1HzTimeout = 0;
    m_ui10HzTimeout = 0;
    m_ui100HzTimeout = 0;
    m_ui1000HzTimeout = 0;
    m_uiLoopCnt = 0;
}

// -------------------------------------------------------------------------

void Application::Initialize()
{
    InitializeComm();

    InitializeObjects();

    InitializeIO();
}

// -------------------------------------------------------------------------

void Application::InitializeObjects()
{

}

// -------------------------------------------------------------------------

void Application::InitializeComm()
{
    Serial.begin(115200);
}

// -------------------------------------------------------------------------

void Application::InitializeIO()
{

}

// -------------------------------------------------------------------------

void Application::Run()
{
    // ... start infinite main program loop.
    while(true) {

        // Process all.
        ProcessLoop();
    }
}

// -------------------------------------------------------------------------

void Application::ProcessLoop(void)
{
    m_uiLoopCnt++;

    // 100Hz = Every millisecond
    if(helper::GetTime() > m_ui1000HzTimeout) {
        m_ui1000HzTimeout = helper::GetTime();
        Process1000Hz();
    }

    // 100Hz = Every 10 milliseconds
    if(helper::GetTimeDifference(m_ui100HzTimeout) >= 10) {
        m_ui100HzTimeout = helper::GetTime();
        Process100Hz();
    }

    // 10Hz = Every 100 milliseconds
    if(helper::GetTimeDifference(m_ui10HzTimeout) >= 100) {
        m_ui10HzTimeout = helper::GetTime();
        Process10Hz();
    }

    // 1Hz = Every second
    uint32_t uiDiff = helper::GetTimeDifference(m_ui1HzTimeout);
    if(uiDiff >= 1000) {
        m_ui1HzTimeout = helper::GetTime();
        Process1Hz();

        // Handle loop count
        SetLoopCount(m_uiLoopCnt);
        m_uiLoopCnt = 0;
    }
}

// -------------------------------------------------------------------------
