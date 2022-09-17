#ifndef NETWORK_H
#define NETWORK_H

#include <WiFiUdp.h>
#include "AsyncUDP.h"
#include <WiFiServer.h>
#include "TypesDef.h"

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

    //! Send UDP datagram to given station
    virtual void Send(uint8_t *auiBuffer, uint16_t uiSize, IPAddress ipAddress, uint16_t uiPort);

    //! Returns true if connected to WiFi
    bool IsConnected()
    {   return m_eState > nsWaitConnected; }

    //! Get network parameters from settings. Reimplement in inherited class.
    virtual void GetNetworkParameters(types::NetworkParameters* sParam) = 0;

protected:
    //! Connects to given access point
    virtual void ConnectToWiFi(char* acSSID, char* acPass);

    //! Parses received UDP datagrams
    virtual void ParseUDP(AsyncUDPPacket &packet);

protected:
    //! Udp handler class
    AsyncUDP  AUdp;

    //! Current state machine state
    NetworkStates m_eState = nsIdle;

    //! Last state machine state change
    uint32_t m_uiLastStateChange = 0;

    //! Strucure holding network settings
    types::NetworkParameters m_sParams;
};

#endif // NETWORK_H
