#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>
#include <stddef.h>
#include "Settings.h"

class Model
{
protected:
    //! Constructor
    Model();

public:
    //! Method creates the only instance of this class and returns pointer to it.
    static Model* GetInstance();

public:
    //! Destructor
    ~Model()
    {   }

    //! Initilize member variables
    virtual void Initialize();

    //! Returns pointer to settings.
    Settings * GetSettings()
    {   return m_pSet; }
	
    //! Sets pointer to settings.
    void SetSettingsPointer(Settings *pSet)
    {   m_pSet = pSet; }

    //! Saves settings - Use this method instead of calling GetSettings()->Save()
    void SaveSettings()
    {	GetSettings()->Save(); }

    //! Returns current main process loop count
    uint32_t GetLoopCount() const
    {   return m_uiLoopCnt; }

    //! Sets current main process loop count
    void SetLoopCount(uint32_t uiValue)
    {   m_uiLoopCnt = uiValue; }
    
protected:
    //! The only static singleton object.
    static Model* s_pInstance;

    //! Pointer to settings
    Settings *m_pSet = nullptr;

    //! Main Process loop counter.
    uint32_t m_uiLoopCnt;
};

#endif // MODEL_H
