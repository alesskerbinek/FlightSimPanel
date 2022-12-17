#ifndef NETWORK_H
#define NETWORK_H

#include <WiFiUdp.h>
#include "AsyncUDP.h"
#include <WiFiServer.h>
#include "TypesDef.h"
#include "XPlaneDef.h"

class Network
{
private:
    enum NetworkStates {
        nsIdle,
        nsConnectWiFi,
        nsWaitConnected,
        nsListening,
        nsListenError,
    };

public:
    //! Public constructor.
    Network();

    //! Public destructor.
    ~Network(){;}

    //! Initializes class.
    virtual void Initialize();

    //! Main state machine loop.
    virtual void Process();

    //! Returns true if connected to WiFi
    bool IsConnected();

    //! Get network parameters from settings. Reimplement in inherited class.
    virtual void GetNetworkParameters(types::NetworkParameters* sParam) = 0;

protected:
    //! Send UDP datagram to given station
    virtual void Send(const uint8_t *auiBuffer, uint16_t uiSize, IPAddress ipAddress, uint16_t uiPort);

    //! Connects to given access point
    virtual void ConnectToWiFi(char* acSSID, char* acPass);

    //! Parses received UDP datagrams
    virtual void ParseUDP(AsyncUDPPacket &packet);

    //! Parses DREF message. Reimplement in inherited class.
    virtual void ParseDREF(uint8_t* pBuffer) {}

    //! Parses COM1 & COM2 frequency DATA message. Reimplement in inherited class.
    virtual void ParseCOM(uint8_t* pBuffer) {}

    //! Parses NAV1 & NAV2 frequency DATA message. Reimplement in inherited class.
    virtual void ParseNAV(uint8_t* pBuffer) {}

    //! Parses NAV1 & NAV2 OBS DATA message. Reimplement in inherited class.
    virtual void ParseOBS(uint8_t* pBuffer) {}

    //! Parses TRANSPONDER DATA message. Reimplement in inherited class.
    virtual void ParseXPDR(uint8_t* pBuffer) {}

    /**
    * @brief Returns text representation for given DataRef type.
    * @param dr DataRef type
    * @return DataRef text
    */
    virtual const char* GetDataRefString(xplane::DataRefs dr);

    /**
     * @brief Returns text representation for given Command type.
     * @param cmd Command type
     * @return Command text
     */
    virtual const char* GetCommandString(xplane::Commands cmd);

    /**
     * @brief Constructs DREF datagram from given attributes.
     * @param uiValue 4 byte value
     * @param eType DataRef type
     */
    virtual void SendDataRef(uint32_t uiValue, xplane::DataRefs eType);

    /**
     * @brief Contructs CHAR datagram with given value
     * @param uiValue keyboard key character
     */
    virtual void SendChar(uint8_t uiValue);

    /**
     * @brief Constructs CMND datagram with given type.
     * @param eType Command type
     */
    virtual void SendCommand(xplane::Commands eType);

    /**
     * @brief Function checks whether there are messages in Tx queue that need to be sent.
     * Sends the oldest datagram in the queue (FIFO).
     * Reimplement in inherited class.
     */
    virtual void ProcessTxQueue() = 0;


protected:
    //! Udp handler class
    AsyncUDP  AUdp;

    //! Current state machine state
    NetworkStates m_eState = nsIdle;

    //! Last state machine state change
    uint32_t m_uiLastStateChange = 0;

    //! Last message received timestamp
    uint32_t m_uiLastRxTime = 0;

    //! Strucure holding network settings
    types::NetworkParameters m_sParams;
};

#endif // NETWORK_H
