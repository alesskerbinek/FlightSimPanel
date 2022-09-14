#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include <stddef.h>
#include <Preferences.h>

#define PREFERENCES_SETTINGS    "settings"

class Settings
{
protected:
        //! Constructor
        Settings();

public:
        //! Public destructor.
        ~Settings() {}

        //! The only access to the singleton object.
        static Settings* GetInstance();

        //! Factory reset settings.
        virtual void Initialize() {}

        //! Initialize whole settings structure
        virtual void InitializeAll() {}

        //! Save current settings.
        virtual void Save() {}

        //! Load current settings.
        virtual void Load() {}

        //! Returns serial number
        uint32_t GetSerialNumber() const
        {   return m_uiSerialNumber; }

        //! Sets serial number
        void SetSerialNumber(uint32_t uiSerial)
        {   m_uiSerialNumber = uiSerial; }

protected:
        //! The only static singleton object.
        static Settings* s_pInstance;

        //! Device serial number
        uint32_t m_uiSerialNumber;

        //! Hold settings in the Flash
        Preferences m_preferences;
};

#endif // SETTINGS_H
