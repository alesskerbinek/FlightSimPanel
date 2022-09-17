#include <WiFi.h>
#include "Helper.h"
#include "Network.h"

// -------------------------------------------------------------------------

Network::Network()
{
    
}

// -------------------------------------------------------------------------

void Network::Initialize()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    GetNetworkParameters(&m_sParams);
}

// -------------------------------------------------------------------------

void Network::Send(uint8_t *auiBuffer, uint16_t uiSize, IPAddress ipAddress, uint16_t uiPort)
{
    //m_sParams.m_uiUdpRemoteIP, m_sParams.m_uiUdpRemotePort

    if(IsConnected()) {
        AUdp.writeTo(auiBuffer, uiSize, ipAddress, uiPort);
    }
}

// -------------------------------------------------------------------------

void Network::ConnectToWiFi(char* acSSID, char* acPass)
{
    WiFi.begin(acSSID, acPass);
}

// -------------------------------------------------------------------------

void Network::ParseUDP(AsyncUDPPacket &packet)
{
    //Packet Type: packet.isBroadcast() packet.isMulticast()
    //From: packet.remoteIP() packet.remotePort()
    //To: packet.localIP()); packet.localPort());
    //Len: packet.length());
    //Data:
    Serial.write(packet.data(), packet.length());


}

// -------------------------------------------------------------------------

void Network::Process()
{
    switch (m_eState) {
    case nsIdle:
        m_eState = nsConnectWiFi;
        m_uiLastStateChange = helper::GetTime();
        break;
    case nsConnectWiFi:
        //ConnectToWiFi("ZANDAnet", "Kraigherjeva7");
        ConnectToWiFi(m_sParams.m_acWiFiSSID, m_sParams.m_acWiFiPass);
        m_eState = nsWaitConnected;
        m_uiLastStateChange = helper::GetTime();
        break;
    case nsWaitConnected:
        if(WiFi.status() == WL_CONNECTED) {
            Serial.printf("$PMDBG,WiFi connected,%s,%i.%i.%i.%i\n", WiFi.SSID(), WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
            if (AUdp.listen(m_sParams.m_uiUdpListenPort)) {
                AUdp.onPacket([this](AsyncUDPPacket packet) {
                    ParseUDP(packet);
                });
                Serial.printf("$PMDBG,UDP listening,%i\n", m_sParams.m_uiUdpListenPort);
                m_eState = nsListening;
            } else {
                Serial.printf("$PMERR,UDP listen fail,%i\n", m_sParams.m_uiUdpListenPort);
                m_eState = nsListenError;
            }
            m_uiLastStateChange = helper::GetTime();
        } else if(WiFi.status() == WL_CONNECT_FAILED
                  || WiFi.status() == WL_NO_SSID_AVAIL
                  || helper::GetTimeDifference(m_uiLastStateChange) > 10000) // 30s timeout for WiFi connect
        {
            Serial.printf("$PMERR,WiFi connect fail,%s,%s\n", m_sParams.m_acWiFiSSID, m_sParams.m_acWiFiPass);
            // Retry
            m_eState = nsConnectWiFi;
            m_uiLastStateChange = helper::GetTime();
        }
        break;
    case nsListening:
        if(WiFi.status() != WL_CONNECTED) {
            Serial.printf("$PMDBG,UDP disconnected,%i\n", WiFi.status());
            // Reconnect
            m_eState = nsConnectWiFi;
            m_uiLastStateChange = helper::GetTime();
        }
        break;
    case nsListenError:
        // Nothing to be done. Wait for system reset... ???
        break;
    default:
        break;
    }




//    uint8_t auiBuffer[20];
//    snprintf((char*)auiBuffer, sizeof(auiBuffer), "ms=%i......", millis());

//    if(m_bConnected == false) {
//        ConnectToWiFi("ZANDAnet", "Kraigherjeva7", 20229);
//    } else {
//        const IPAddress addr = IPAddress(192,168,8,129);
//        Send(auiBuffer, addr, 49672);
//    }
}

// -------------------------------------------------------------------------
