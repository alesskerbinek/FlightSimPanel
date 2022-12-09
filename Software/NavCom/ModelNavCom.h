#ifndef MODELNAVCOM_H
#define MODELNAVCOM_H

#include <stdint.h>
#include <stddef.h>
#include <utility>
#include "Model.h"
#include "SettingsNavCom.h"

class ModelNavCom : public Model
{
protected:
    //! Constructor
    ModelNavCom();

public:
    //! Destructor
    ~ModelNavCom() { }

    //! The only access to the singleton object.
    static ModelNavCom* GetInstance();

    //! Initilize member variables
    virtual void Initialize() override;

    //! Returns pointer to settings.
    SettingsNavCom * GetSettings()
    {   return m_pSetNavCom; }

    //! Returns pointer to settings.
    const SettingsNavCom * GetSettings() const
    {   return m_pSetNavCom; }

    //! Sets pointer to settings.
    void SetSettingsPointer(SettingsNavCom *pSet)
    {
        m_pSetNavCom = pSet;
        m_pSet = pSet;
    }

    //! Returns current transponder mode
    UnitModes GetUnitMode() const
    {   return m_eUnitMode; }

    //! Sets current transponder mode
    void SetUnitMode(UnitModes eMode)
    {   m_eUnitMode = eMode; }

    //! Sets transponder ident
    void SetXpndrIdent();

    //! Returns true if transponder is emitting ident
    bool IsXpndrIdent();

    //! Sets transponder shutdown - wait and check that Off button is
    //! pressed for couple of seconds then change mode to Off.
    void SetXpndrShutdown(bool bShutdown);

    //! Returns true if transponder is in shutdown state
    bool IsXpndrShutdown();

    //! Returns current active value on display
    uint32_t GetActiveValue() const;

    //! Sets current active value on display
    void SetActiveValue(uint32_t uiValue);

    //! Increases/decreases current active value on display
    void IncrActiveValue(int32_t iIncrement, uint32_t uiOverflow);

    //! Returns current standby value on display
    uint32_t GetStandbyValue() const;

    //! Sets current standby value on display
    void SetStandbyValue(uint32_t uiValue);

    //! Increases/decreases current standby value on display
    void IncrStandbyValue(int32_t iIncrement, uint32_t uiOverflow);

    //! Returns this unit type
    UnitTypes GetUnitType()
    {   return m_pSetNavCom ? m_pSetNavCom->GetUnitType() : utUndefined; }

    //! Returns current standby value on display
    uint8_t GetVolume() const;

    //! Sets current volume value on display
    void SetVolume(int8_t iValue);

    //! Returns true if volume setting is in progress
    bool IsVolumeCommand();

    //! Returns true if value editing currently in progress.
    bool IsEditingDigit() const;

    //! Returns currently editing value first and last digit.
    std::pair<int16_t, int16_t> GetEditingDigit() const;

    //! Sets currently editing value first and last digit.
    void SetEditingDigit(int16_t iSingleDigit);

    //! Sets currently editing value first and last digit.
    void SetEditingDigit(int16_t iStartDigit, int16_t iEndDigit);

    //! Terminates value editing indication on display.
    void SetEditingFinished();

    //! Reseturns flight time timer value in seconds.
    uint32_t GetFlightTime();

    //! Reseturns elapsed time timer value in seconds.
    uint32_t GetElapsedTime();

    //! Resets eleapsed time timer.
    void ResetET();

protected:
    //! Returns uiValue incremented for given iIncr and overflows digit at uiOver.
    uint32_t IncrementDigit(uint32_t uiValue, int32_t iIncr, uint32_t uiOver);

protected:
    //! The only static singleton object.
    static ModelNavCom* s_pInstanceNavCom;

    //! Pointer to settings
    SettingsNavCom *m_pSetNavCom = nullptr;

    //! Maximum Active/Standby value
    uint32_t m_uiMaxActive = 136000;

    //! Minimum Active/Standby value
    uint32_t m_uiMinActive = 0;

    //! Current transponder mode
    UnitModes m_eUnitMode = gmOff;

    //! Timestamp of last ident command
    uint32_t m_uiXpndrIdentTimestamp = 0;

    //! Set transponder for shutdown
    bool m_bXpndrShutdown = false;

    //! Unit volume [0-100%]
    uint8_t m_uiVolume = 0;

    //! Timestamp of last volume command
    uint32_t m_uiVolumeTimestamp = 0;

    //! First and last digit of currently editing value
    std::pair<int16_t, int16_t> m_pairEditingDigit = std::make_pair(1,0);

    //! Timestamp of last value editing command
    uint32_t m_uiValueEditTimestamp = 0;

    //! Timestamp of last Elapsed timer reset
    uint32_t m_uiETResetTimestamp = 0;
};

#endif  // MODELNAVCOM_H
