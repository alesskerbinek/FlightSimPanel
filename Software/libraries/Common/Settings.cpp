#include "Settings.h"

Settings* Settings::s_pInstance = NULL;

// -------------------------------------------------------------------------

Settings::Settings()
{
    m_uiSerialNumber = 0;

    m_preferences.begin(PREFERENCES_SETTINGS); // use "settings" namespace

    Initialize();
}

// -------------------------------------------------------------------------

Settings* Settings::GetInstance()
{
        if(s_pInstance==NULL)
                s_pInstance = new Settings();

        return s_pInstance;
}

// -------------------------------------------------------------------------
