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

void Network::Send(const uint8_t *auiBuffer, uint16_t uiSize, IPAddress ipAddress, uint16_t uiPort)
{
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
    if(strncmp((char*)packet.data(), "DATA", 4) == 0) {
        for(int i = 5; i < packet.length(); i += 4+8*4) {
            switch (packet.data()[i]) {
            case xplane::idComFreq:
                ParseCOM(&packet.data()[i]);
                break;
            case xplane::idNavFreq:
                ParseNAV(&packet.data()[i]);
                break;
            case xplane::idNavObs:
                ParseOBS(&packet.data()[i]);
                break;
            default:
                break;
            }
        }
    }
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
        ConnectToWiFi(m_sParams.m_acWiFiSSID, m_sParams.m_acWiFiPass);
        m_eState = nsWaitConnected;
        m_uiLastStateChange = helper::GetTime();
        break;
    case nsWaitConnected:
        if(WiFi.status() == WL_CONNECTED) {
            Serial.printf("$PMDBG,WiFi connected,%s,%i.%i.%i.%i\r\n", WiFi.SSID(), WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
            if (AUdp.listen(m_sParams.m_iUdpListenPort)) {
                AUdp.onPacket([this](AsyncUDPPacket packet) {
                    ParseUDP(packet);
                });
                Serial.printf("$PMDBG,UDP listening,%i\r\n", m_sParams.m_iUdpListenPort);
                m_eState = nsListening;
            } else {
                Serial.printf("$PMERR,UDP listen fail,%i\r\n", m_sParams.m_iUdpListenPort);
                m_eState = nsListenError;
            }
            m_uiLastStateChange = helper::GetTime();
        } else if(WiFi.status() == WL_CONNECT_FAILED
                  || WiFi.status() == WL_NO_SSID_AVAIL
                  || helper::GetTimeDifference(m_uiLastStateChange) > 10000) // 30s timeout for WiFi connect
        {
            Serial.printf("$PMERR,WiFi connect fail,%s,%s\r\n", m_sParams.m_acWiFiSSID, m_sParams.m_acWiFiPass);
            // Retry
            m_eState = nsConnectWiFi;
            m_uiLastStateChange = helper::GetTime();
        }
        break;
    case nsListening:
        if(WiFi.status() != WL_CONNECTED) {
            Serial.printf("$PMDBG,UDP disconnected,%i\r\n", WiFi.status());
            // Reconnect
            m_eState = nsConnectWiFi;
            m_uiLastStateChange = helper::GetTime();
        }
        ProcessTxQueue();
        break;
    case nsListenError:
        // Nothing to be done. Wait for system reset... ???
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------------

void Network::ProcessTxQueue()
{
    if(m_qTX.empty() == false)
    {
        xplane::UdpDatagram sDatagram = m_qTX.front();
        m_qTX.pop();

        switch (sDatagram.m_eType) {
        case xplane::dtDATA:
            // Implement if needed...
            break;
        case xplane::dtDREF:
            SendDataRef(sDatagram.m_uiValue, static_cast<xplane::DataRefs>(sDatagram.m_uiParameter));
            break;
        case xplane::dtCHAR:
            SendChar(sDatagram.m_uiValue & 0xFF);
            break;
        case xplane::dtCMND:
            SendCommand(static_cast<xplane::Commands>(sDatagram.m_uiParameter));
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------------------------

const char* Network::GetDataRefString(xplane::DataRefs dr)
{
    switch (dr) {
    case xplane::drCom1Freq:
        return "/sim/cockpit2/radios/actuators/com1_standby_frequency_hz[0]";
    default:
        return nullptr;
    }
}

// -------------------------------------------------------------------------

const char* Network::GetCommandString(xplane::Commands cmd)
{
    switch (cmd) {
    case xplane::cmLandingGear:
        return "/sim/flight_controls/landing_gear_down";
    default:
        return nullptr;
    }
}

// -------------------------------------------------------------------------

void Network::SendDataRef(uint32_t uiValue, xplane::DataRefs eType)
{
    const char* pText = GetDataRefString(eType);
    if(pText) {
        char acBuffer[509];
        strncpy(acBuffer, "DREF0", 5);

        memcpy(&acBuffer[5], &uiValue, 4);

        memset(&acBuffer[9], 0x20, sizeof(acBuffer)-9);
        strncpy(&acBuffer[9], pText, sizeof(acBuffer)-9);

        Send((const uint8_t*)acBuffer, 509, IPAddress(m_sParams.m_uiUdpRemoteIP), m_sParams.m_iUdpRemotePort);
    }
}

// -------------------------------------------------------------------------

void Network::SendChar(uint8_t uiValue)
{
    char acBuffer[6];
    strncpy(acBuffer, "CHAR0", 5);
    acBuffer[5] = uiValue;

    Send((const uint8_t*)acBuffer, 6, IPAddress(m_sParams.m_uiUdpRemoteIP), m_sParams.m_iUdpRemotePort);
}

// -------------------------------------------------------------------------

void Network::SendCommand(xplane::Commands eType)
{
    const char* pText = GetCommandString(eType);
    if(pText) {
        char acBuffer[100];
        strncpy(acBuffer, "CMND0", 5);

        strncpy(&acBuffer[5], pText, sizeof(acBuffer)-5);

        Send((const uint8_t*)acBuffer, strlen(acBuffer), IPAddress(m_sParams.m_uiUdpRemoteIP), m_sParams.m_iUdpRemotePort);
    }
}

// -------------------------------------------------------------------------

void Network::AddToTxQueue(xplane::UdpDatagram data)
{
    m_qTX.push(data);
}

// -------------------------------------------------------------------------
