#ifndef SETTINGSNAVCOM_H
#define SETTINGSNAVCOM_H

#include "ApplicationDefines.h"
#include <Settings.h>

class SettingsNavCom : public Settings
{
protected:
        //! Constructor
        SettingsNavCom();

public:
        //! Public destructor.
        ~SettingsNavCom() {}

        //! The only access to the singleton object.
        static SettingsNavCom* GetInstance();

        //! Factory reset settings.
        virtual void Initialize() override;

        //! Initialize whole settings structure
        virtual void InitializeAll() override;

        //! Save current settings.
        virtual void Save() override;

        //! Load current settings.
        virtual void Load() override;

        //! Returns current active value on display
        uint32_t GetActiveValue() const
        {   return m_uiActiveValue; }

        //! Sets current active value on display
        void SetActiveValue(uint32_t uiValue)
        {   m_uiActiveValue = uiValue; }

        //! Returns current standby value on display
        uint32_t GetStandbyValue() const
        {   return m_uiStandbyValue; }

        //! Sets current standby value on display
        void SetStandbyValue(uint32_t uiValue)
        {   m_uiStandbyValue = uiValue; }

        //! Returns current OBS value on display
        uint16_t GetOBS() const
        {   return m_uiOBS; }

        //! Sets current OBS value on display
        void SetOBS(uint16_t uiValue)
        {   m_uiOBS = uiValue; }

        //! Returns this unit type
        UnitTypes GetUnitType()
        {   return m_eUnitType; }

        //! Sets this unit type
        void SetUnitType(UnitTypes eType)
        {   m_eUnitType = eType; }

private:
        //! Copy values to member variables
        void CopyToVariables(settings_t *pSettings);

protected:
        //! The only static singleton object.
        static SettingsNavCom* s_pInstanceNavCom;

        //! Last active frequency or squawk
        uint32_t m_uiActiveValue = 0;

        //! Last standby frequency or squawk
        uint32_t m_uiStandbyValue = 0;

        //! Last OBS value
        uint16_t m_uiOBS = 0;

        //! This unit type
        UnitTypes m_eUnitType = utUndefined;
};

#endif // SETTINGSNAVCOM_H
