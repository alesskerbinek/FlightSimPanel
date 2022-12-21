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

bool Network::IsConnected()
{
    // Declare we are connected if UDP server is established + we received any message in last 3s.
    return m_eState == nsListening
            && m_uiLastRxTime > 0
            && helper::GetTimeDifference(m_uiLastRxTime) < 3000;
}

// -------------------------------------------------------------------------

void Network::Send(const uint8_t *auiBuffer, uint16_t uiSize, IPAddress ipAddress, uint16_t uiPort)
{
    if(m_eState > nsWaitConnected) {
        //Serial.printf("Send %s, %i\n", (const char*)auiBuffer, uiSize);
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
    m_uiLastRxTime = helper::GetTime();

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
            case xplane::idXpndrStatus:
                ParseXPDR(&packet.data()[i]);
                break;
            case xplane::idAdfStatus:
                ParseADF(&packet.data()[i]);
                break;
            default:
                break;
            }
        }
    }
    else if(strncmp((char*)packet.data(), "DREF", 4) == 0) {
        // X-Plane has a bug. When you enable and/or change which dataref are to be output
        // you have to reboot simulator otherwise the value is always 0.
        ParseDREF(packet.data());
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

const char* Network::GetDataRefString(xplane::DataRefs dr)
{
    switch (dr) {
    case xplane::drNav1StandbyHz:
        return "sim/cockpit2/radios/actuators/nav_frequency_hz";
    case xplane::drNav1StandbyMHz:
        return "sim/cockpit2/radios/actuators/nav1_standby_frequency_Mhz";
    case xplane::drNav1StandbykHz:
        return "sim/cockpit2/radios/actuators/nav1_standby_frequency_khz";
    case xplane::drCom1ActiveFreq:
        return "sim/cockpit2/radios/actuators/com1_frequency_hz_833";           // OK
    case xplane::drCom1StandbyFreq:
        return "sim/cockpit2/radios/actuators/com1_standby_frequency_hz_833";   // OK
    case xplane::drCom2ActiveFreq:
        return "sim/cockpit2/radios/actuators/com2_frequency_hz_833";           // OK
    case xplane::drCom2StandbyFreq:
        return "sim/cockpit2/radios/actuators/com2_standby_frequency_hz_833";   // OK
    case xplane::drAdf1ActiveFreq:
        return "sim/cockpit/radios/adf1_freq_hz";                               // OK
    case xplane::drAdf1StandbyFreq:
        return "sim/cockpit/radios/adf1_stdby_freq_hz";                         // OK
    case xplane::drAdf2ActiveFreq:
        return "sim/cockpit/radios/adf2_freq_hz";
    case xplane::drAdf2StandbyFreq:
        return "sim/cockpit/radios/adf2_stdby_freq_hz";
    case xplane::drXpdrCode:
        return "sim/cockpit/radios/transponder_code";                           // OK
    default:
        return nullptr;
    }
}

// -------------------------------------------------------------------------

const char* Network::GetCommandString(xplane::Commands cmd)
{
    switch (cmd) {
    case xplane::cmNav1Flip:
        return "sim/radios/nav1_standy_flip";           // OK
    case xplane::cmNav2Flip:
        return "sim/radios/nav2_standy_flip";           // OK
    case xplane::cmCom1Flip:
        return "sim/radios/com1_standy_flip";           // OK
    case xplane::cmCom2Flip:
        return "sim/radios/com2_standy_flip";           // OK
    case xplane::cmAdf1Flip:
        return "sim/radios/adf1_standy_flip";           // OK
    case xplane::cmAdf2Flip:
        return "sim/radios/adf2_standy_flip";
    case xplane::cmDmeFlip:
        return "sim/radios/dme_standy_flip";
    case xplane::cmNav1StandbyCoarseUp:
        return "sim/radios/stby_nav1_coarse_up";        // OK
    case xplane::cmNav1StandbyCoarseDown:
        return "sim/radios/stby_nav1_coarse_down";      // OK
    case xplane::cmNav1StandbyFineUp:
        return "sim/radios/stby_nav1_fine_up";          // OK
    case xplane::cmNav1StandbyFineDown:
        return "sim/radios/stby_nav1_fine_down";        // OK
    case xplane::cmNav2StandbyCoarseUp:
        return "sim/radios/stby_nav2_coarse_up";        // OK
    case xplane::cmNav2StandbyCoarseDown:
        return "sim/radios/stby_nav2_coarse_down";      // OK
    case xplane::cmNav2StandbyFineUp:
        return "sim/radios/stby_nav2_fine_up";          // OK
    case xplane::cmNav2StandbyFineDown:
        return "sim/radios/stby_nav2_fine_down";        // OK
    case xplane::cmCom1StandbyCoarseUp:
        return "sim/radios/stby_com1_coarse_up";        // OK
    case xplane::cmCom1StandbyCoarseDown:
        return "sim/radios/stby_com1_coarse_down";      // OK
    case xplane::cmCom1StandbyFineUp:
        return "sim/radios/stby_com1_fine_up";          // OK
    case xplane::cmCom1StandbyFineDown:
        return "sim/radios/stby_com1_fine_down";        // OK
    case xplane::cmCom1Standby833Up:
        return "sim/radios/stby_com1_fine_up_833";      // OK
    case xplane::cmCom1Standby833Down:
        return "sim/radios/stby_com1_fine_down_833";    // OK
    case xplane::cmCom2StandbyCoarseUp:
        return "sim/radios/stby_com2_coarse_up";        // OK
    case xplane::cmCom2StandbyCoarseDown:
        return "sim/radios/stby_com2_coarse_down";      // OK
    case xplane::cmCom2StandbyFineUp:
        return "sim/radios/stby_com2_fine_up";          // OK
    case xplane::cmCom2StandbyFineDown:
        return "sim/radios/stby_com2_fine_down";        // OK
    case xplane::cmCom2Standby833Up:
        return "sim/radios/stby_com2_fine_up_833";      // OK
    case xplane::cmCom2Standby833Down:
        return "sim/radios/stby_com2_fine_down_833";    // OK
    case xplane::cmCom1On:
        return "sim/radios/power_com1_on";              // X
    case xplane::cmCom1Off:
        return "sim/radios/power_com1_off";             // X
    case xplane::cmCom2On:
        return "sim/radios/power_com2_on";              // X
    case xplane::cmCom2Off:
        return "sim/radios/power_com2_off";             // X
    case xplane::cmNav1ObsUp:
        return "sim/radios/obs1_up";
    case xplane::cmNav1ObsDown:
        return "sim/radios/obs1_down";
    case xplane::cmNav2ObsUp:
        return "sim/radios/obs2_up";
    case xplane::cmNav2ObsDown:
        return "sim/radios/obs2_down";
    case xplane::cmXpdrX000Up:
        return "sim/transponder/transponder_thousands_up";      // OK
    case xplane::cmXpdrX000Down:
        return "sim/transponder/transponder_thousands_down";    // OK
    case xplane::cmXpdr0X00Up:
        return "sim/transponder/transponder_hundreds_up";       // OK
    case xplane::cmXpdr0X00Down:
        return "sim/transponder/transponder_hundreds_down";     // OK
    case xplane::cmXpdr00X0Up:
        return "sim/transponder/transponder_tens_up";           // OK
    case xplane::cmXpdr00X0Down:
        return "sim/transponder/transponder_tens_down";         // OK
    case xplane::cmXpdr000XUp:
        return "sim/transponder/transponder_ones_up";           // OK
    case xplane::cmXpdr000XDown:
        return "sim/transponder/transponder_ones_down";         // OK
    case xplane::cmXpdrIdent:
        return "sim/transponder/transponder_ident";             // OK
    case xplane::cmXpdrOff:
        return "sim/transponder/transponder_off";               // OK
    case xplane::cmXpdrSby:
        return "sim/transponder/transponder_standby";           // OK
    case xplane::cmXpdrOn:
        return "sim/transponder/transponder_on";                // OK
    case xplane::cmXpdrAlt:
        return "sim/transponder/transponder_alt";               // OK
    case xplane::cmAdf1ActiveXX00Up:
        return "sim/radios/actv_adf1_hundreds_thous_up";        // OK
    case xplane::cmAdf1ActiveXX00Down:
        return "sim/radios/actv_adf1_hundreds_thous_down";      // OK
    case xplane::cmAdf1Active00X0Up:
        return "sim/radios/actv_adf1_tens_up";                  // OK
    case xplane::cmAdf1Active00X0Down:
        return "sim/radios/actv_adf1_tens_down";                // OK
    case xplane::cmAdf1Active000XUp:
        return "sim/radios/actv_adf1_ones_up";                  // OK
    case xplane::cmAdf1Active000XDown:
        return "sim/radios/actv_adf1_ones_down";                // OK
    case xplane::cmAdf1StandbyXX00Up:
        return "sim/radios/stby_adf1_hundreds_thous_up";        // OK
    case xplane::cmAdf1StandbyXX00Down:
        return "sim/radios/stby_adf1_hundreds_thous_down";      // OK
    case xplane::cmAdf1Standby00X0Up:
        return "sim/radios/stby_adf1_tens_up";                  // OK
    case xplane::cmAdf1Standby00X0Down:
        return "sim/radios/stby_adf1_tens_down";                // OK
    case xplane::cmAdf1Standby000XUp:
        return "sim/radios/stby_adf1_ones_up";                  // OK
    case xplane::cmAdf1Standby000XDown:
        return "sim/radios/stby_adf1_ones_down";                // OK
    case xplane::cmAdf2ActiveXX00Up:
        return "sim/radios/actv_adf2_hundreds_thous_up";
    case xplane::cmAdf2ActiveXX00Down:
        return "sim/radios/actv_adf2_hundreds_thous_down";
    case xplane::cmAdf2Active00X0Up:
        return "sim/radios/actv_adf2_tens_up";
    case xplane::cmAdf2Active00X0Down:
        return "sim/radios/actv_adf2_tens_down";
    case xplane::cmAdf2Active000XUp:
        return "sim/radios/actv_adf2_ones_up";
    case xplane::cmAdf2Active000XDown:
        return "sim/radios/actv_adf2_ones_down";
    case xplane::cmAdf2StandbyXX00Up:
        return "sim/radios/stby_adf2_hundreds_thous_up";
    case xplane::cmAdf2StandbyXX00Down:
        return "sim/radios/stby_adf2_hundreds_thous_down";
    case xplane::cmAdf2Standby00X0Up:
        return "sim/radios/stby_adf2_tens_up";
    case xplane::cmAdf2Standby00X0Down:
        return "sim/radios/stby_adf2_tens_down";
    case xplane::cmAdf2Standby000XUp:
        return "sim/radios/stby_adf2_ones_up";
    case xplane::cmAdf2Standby000XDown:
        return "sim/radios/stby_adf2_ones_down";
    case xplane::cmAdf1PowerOff:
        return "sim/radios/adf1_power_mode_0";          // OK
    case xplane::cmAdf1PowerAnt:
        return "sim/radios/adf1_power_mode_1";          // OK
    case xplane::cmAdf1PowerOn:
        return "sim/radios/adf1_power_mode_2";          // OK
    case xplane::cmAdf1PowerTone:
        return "sim/radios/adf1_power_mode_3";          // OK
    case xplane::cmAdf1PowerTest:
        return "sim/radios/adf1_power_mode_4";
    case xplane::cmAdf2PowerOff:
        return "sim/radios/adf2_power_mode_0";
    case xplane::cmAdf2PowerAnt:
        return "sim/radios/adf2_power_mode_1";
    case xplane::cmAdf2PowerOn:
        return "sim/radios/adf2_power_mode_2";
    case xplane::cmAdf2PowerTone:
        return "sim/radios/adf2_power_mode_3";
    case xplane::cmAdf2PowerTest:
        return "sim/radios/adf2_power_mode_4";
    case xplane::cmLandingGear:
        return "sim/flight_controls/landing_gear_down";
    case xplane::cmFlapsUp:
        return "sim/flight_controls/flaps_up";          // OK
    case xplane::cmFlapsDown:
        return "sim/flight_controls/flaps_down";        // OK
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
        strncpy(acBuffer, "DREF\0", 5);

        memcpy(&acBuffer[5], &uiValue, 4);

        memset(&acBuffer[9], 0x20, sizeof(acBuffer)-9);
        strncpy(&acBuffer[9], pText, sizeof(acBuffer)-9); // pText has to be NULL terminated

        IPAddress ipAddress = helper::GetIPAddress(m_sParams.m_uiUdpRemoteIP);
        Send((const uint8_t*)acBuffer, 509, ipAddress, m_sParams.m_iUdpRemotePort);
    }
}

// -------------------------------------------------------------------------

void Network::SendChar(uint8_t uiValue)
{
    char acBuffer[6];
    strncpy(acBuffer, "CHAR\0", 5);
    acBuffer[5] = uiValue;

    IPAddress ipAddress = helper::GetIPAddress(m_sParams.m_uiUdpRemoteIP);
    Send((const uint8_t*)acBuffer, 6, ipAddress, m_sParams.m_iUdpRemotePort);
}

// -------------------------------------------------------------------------

void Network::SendCommand(xplane::Commands eType)
{
    const char* pText = GetCommandString(eType);
    if(pText) {
        char acBuffer[100];
        strncpy(acBuffer, "CMND\0", 5);
        //acBuffer[4] = '\0';

        strncpy(&acBuffer[5], pText, sizeof(acBuffer)-5);

        IPAddress ipAddress = helper::GetIPAddress(m_sParams.m_uiUdpRemoteIP);
        Send((const uint8_t*)acBuffer, 5+strlen(pText)+1, ipAddress, m_sParams.m_iUdpRemotePort);
    }
}

// -------------------------------------------------------------------------
