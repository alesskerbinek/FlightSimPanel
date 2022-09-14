#include "SettingsNavCom.h"

SettingsNavCom* SettingsNavCom::s_pInstanceNavCom = NULL;

// -------------------------------------------------------------------------

SettingsNavCom::SettingsNavCom() : Settings()
{

}

//--------------------------------------------------------------------------

SettingsNavCom* SettingsNavCom::GetInstance()
{
    if (s_pInstanceNavCom == NULL) {
            s_pInstanceNavCom = new SettingsNavCom();
            s_pInstance = s_pInstanceNavCom;
    }

    return s_pInstanceNavCom;
}

// -------------------------------------------------------------------------

void SettingsNavCom::Initialize()
{
    // Only settings that can be reset by user - factory reset
    m_uiActiveValue = 0;
    m_uiStandbyValue = 0;
}

// -------------------------------------------------------------------------

void SettingsNavCom::Save()
{
    settings_t rSettings;
    rSettings.uiSerialNumber    = m_uiSerialNumber;
    rSettings.uiActiveValue     = m_uiActiveValue;
    rSettings.uiStandbyValue    = m_uiStandbyValue;
    rSettings.uiUnitType        = static_cast<uint8_t>(m_eUnitType);
	// IMPORTANT!!! When new setting is added, it has to be added here too

    // Clear the rest of settings - we assure initialization this way
    memset(rSettings.auiFree, 0, sizeof(rSettings.auiFree));

    // force the struct array into a byte array
    m_preferences.putBytes(PREFERENCES_SETTINGS, &rSettings, sizeof(settings_t));
}

// -------------------------------------------------------------------------

void SettingsNavCom::Load()
{
    size_t schLen = m_preferences.getBytesLength(PREFERENCES_SETTINGS);
    char acBuffer[schLen]; // prepare a buffer for the data
    m_preferences.getBytes(PREFERENCES_SETTINGS, acBuffer, schLen);
    if (schLen != sizeof(settings_t)) { // simple check that data fits
        Serial.printf("$PMERR,Settings,size missmatch\n");
        return;
    }
    settings_t *pSettings = (settings_t *) acBuffer; // cast the bytes into a struct ptr

    if(sizeof(settings_t) != 256) {
        Serial.printf("$PMERR,Settings,size: %i but should be 256\n", sizeof(settings_t));
    }
    static_assert(sizeof(settings_t) == 256, "Settings size missmatch");

    CopyToVariables(pSettings);
}

// -------------------------------------------------------------------------

void SettingsNavCom::CopyToVariables(settings_t *pSettings)
{
    m_uiSerialNumber    = pSettings->uiSerialNumber;
    m_uiActiveValue     = pSettings->uiActiveValue;
    m_uiStandbyValue    = pSettings->uiStandbyValue;
    m_eUnitType         = static_cast<UnitTypes>(pSettings->uiUnitType);
    // IMPORTANT!!! When new setting is added, it has to be added here too
}

// -------------------------------------------------------------------------

void SettingsNavCom::InitializeAll()
{
    char acBuffer[sizeof(settings_t)]; // prepare a buffer for the data

    memset(acBuffer, 0, sizeof(acBuffer));

    CopyToVariables(reinterpret_cast<settings_t *>(acBuffer));
}

// -------------------------------------------------------------------------
