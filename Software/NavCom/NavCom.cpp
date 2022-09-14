#include "Helper.h"
#include "NavCom.h"

// -------------------------------------------------------------------------

NavCom::NavCom() : Application()
{
    // Object pointers
    m_pSet = nullptr;
    m_pModel = nullptr;
    m_pNetwork = nullptr;
    m_pConfig = nullptr;

    // App variables
}

// -------------------------------------------------------------------------

void NavCom::InitializeObjects()
{
    // Load settings
    m_pSet = SettingsNavCom::GetInstance();
    m_pSet->Load();

    // Create model
    m_pModel = ModelNavCom::GetInstance();
    m_pModel->SetSettingsPointer(m_pSet);
    m_pModel->Initialize();

    // Network manager
    m_pNetwork = new NetworkNavCom();
    m_pNetwork->Initialize();

    // Configuration port
    m_pConfig = new ConfigPortNavCom();
    m_pConfig->Initialize();

    // Input handler
    m_pInput = new Input();

    // Output handler
    m_pOutput = new Output();
}

// -------------------------------------------------------------------------

void NavCom::InitializeIO()
{
    // Buttons
    pinMode(PIN_SW1, INPUT);
    pinMode(PIN_SW2, INPUT);
    pinMode(PIN_SW3, INPUT);
    pinMode(PIN_SW4, INPUT);
    pinMode(PIN_SW5, INPUT);

    // Digit select
    pinMode(DIG_SEL_A, OUTPUT);
    pinMode(DIG_SEL_B, OUTPUT);
    pinMode(DIG_SEL_C, OUTPUT);
    pinMode(DIG_SEL_D, OUTPUT);

    // 7 segment display bus
    pinMode(SEG_A, OUTPUT);
    pinMode(SEG_B, OUTPUT);
    pinMode(SEG_C, OUTPUT);
    pinMode(SEG_D, OUTPUT);
    pinMode(SEG_E, OUTPUT);
    pinMode(SEG_F, OUTPUT);
    pinMode(SEG_G, OUTPUT);
    pinMode(SEG_DP, OUTPUT);

    // Rotary encoders
    pinMode(ROT1_A, INPUT); // Keep this setup even if
    pinMode(ROT1_B, INPUT); // ROT2 is replaced by
    pinMode(ROT2_A, INPUT); // additional switches
    pinMode(ROT2_B, INPUT); // or potentiometer!

    // Initialize Input and Output objects not eariler than the pins functions are set.
    m_pInput->Initialize();
    m_pOutput->Initialize();
}

// -------------------------------------------------------------------------

void NavCom::ProcessLoop(void)
{
    Application::ProcessLoop();

    if(m_pConfig) m_pConfig->Process();
    if(m_pNetwork) m_pNetwork->Process();
}

// -------------------------------------------------------------------------

void NavCom::Process1Hz()
{

}

// -------------------------------------------------------------------------

void NavCom::Process10Hz()
{
    if(m_pOutput) m_pOutput->UpdateValues();
}

// -------------------------------------------------------------------------

void NavCom::Process100Hz()
{

}

// -------------------------------------------------------------------------

void NavCom::Process1000Hz()
{
    if(helper::GetTime() > 1000) {
        if(m_pInput) m_pInput->Process();
    }

    if(m_pOutput) m_pOutput->Process();
}

// -------------------------------------------------------------------------

void NavCom::SetLoopCount(uint32_t uiCount)
{
    if(m_pModel) m_pModel->SetLoopCount(uiCount);
}

// -------------------------------------------------------------------------
