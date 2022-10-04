#ifndef MODEL_H
#define MODEL_H

#include <queue>
#include <cstring>
#include <stdint.h>
#include <stddef.h>
#include "XPlaneDef.h"
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

    //! Returns true if connected to X-Plane
    bool IsSimConnected() const
    {   return m_bIsSimConnected; }

    //! Sets connected (to X-Plane) flag
    void SetSimConnected(bool bConnected)
    {   m_bIsSimConnected = bConnected; }

    /**
     * @brief Adds given datagram to transmit queue
     * @param data datagram to be put on queue.
     */
    void AddToTxQueue(xplane::UdpDatagram data);

    /**
    @brief Returns the oldest datagram from transmit queue (FIFO).
    @return The oldest datagram
    */
    xplane::UdpDatagram GetFromTxQueue();

    /**
     * @brief Returns true if tx queue is empty.
     * @return true if empty
     */
    bool IsTxQueueEmpty() const
    {   return m_qTX.empty(); }

protected:
    //! The only static singleton object.
    static Model* s_pInstance;

    //! Pointer to settings
    Settings *m_pSet = nullptr;

    //! Main Process loop counter.
    uint32_t m_uiLoopCnt;

    //! Flag indicating whenter we are connected to X-Plane or not
    bool m_bIsSimConnected;

    //! UDP Transmit queue.
    std::queue<xplane::UdpDatagram> m_qTX;
};

#endif // MODEL_H
