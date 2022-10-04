#ifndef MODELNAVCOM_H
#define MODELNAVCOM_H

#include <stdint.h>
#include <stddef.h>
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
    XpndrModes GetXpndrMode() const
    {   return m_eXpndrMode; }

    //! Sets current transponder mode
    void SetXpndrMode(XpndrModes eMode)
    {   m_eXpndrMode = eMode; }

    //! Sets transponder ident
    void SetXpndrIdent();

    //! Returns true if transponder is emitting ident
    bool IsXpndrIdent();

    //! Returns current active value on display
    uint32_t GetActiveValue() const;

    //! Sets current active value on display
    void SetActiveValue(uint32_t uiValue);

    //! Returns current standby value on display
    uint32_t GetStandbyValue() const;

    //! Sets current standby value on display
    void SetStandbyValue(uint32_t uiValue);

    //! Increases/decreases current standby value on display
    void IncrStandbyValue(int32_t iIncrement);

    //! Returns this unit type
    UnitTypes GetUnitType()
    {   return m_pSetNavCom ? m_pSetNavCom->GetUnitType() : utUndefined; }

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
    XpndrModes m_eXpndrMode = xmOff;

    //! Current transponder mode
    uint32_t m_uiXpndrIdentTimestamp = 0;
};

#endif  // MODELNAVCOM_H
