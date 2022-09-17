#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include <stddef.h>
#include <Preferences.h>

#define PREFERENCES_SETTINGS    "settings"

class Settings
{
protected:
        //! Constructor
        Settings();

public:
        //! Public destructor.
        ~Settings() {}

        //! The only access to the singleton object.
        static Settings* GetInstance();

        //! Factory reset settings.
        virtual void Initialize() {}

        //! Initialize whole settings structure
        virtual void InitializeAll() {}

        //! Save current settings.
        virtual void Save() {}

        //! Load current settings.
        virtual void Load() {}

        //! Returns serial number
        uint32_t GetSerialNumber() const
        {   return m_uiSerialNumber; }

        //! Sets serial number
        void SetSerialNumber(uint32_t uiSerial)
        {   m_uiSerialNumber = uiSerial; }

        //! Returns UDP listen port
        uint16_t GetListenPort() const
        {   return m_uiUdpListenPort; }

        //! Sets UDP listen port
        void SetListenPort(uint16_t uiPort)
        {   m_uiUdpListenPort = uiPort; }

        //! Returns UDP remote port
        uint16_t GetRemotePort() const
        {   return m_uiUdpRemotePort; }

        //! Sets UDP remote port
        void SetRemotePort(uint16_t uiPort)
        {   m_uiUdpRemotePort = uiPort; }

        //! Returns UDP remote IP
        uint32_t GetRemoteIP() const
        {   return m_uiUdpRemoteIP; }

        //! Sets UDP remote IP
        void SetRemoteIP(uint32_t uiIP)
        {   m_uiUdpRemoteIP = uiIP; }

        //! Returns WiFi SSID to connect to. Null terminated.
        const char* GetWiFiSSID() const
        {   return m_acWiFiSSID; }

        //! Returns WiFi SSID to connect to. Null terminated.
        char* GetWiFiSSID()
        {   return m_acWiFiSSID; }

        //! Returns WiFi SSID size.
        uint16_t GetWiFiSSIDSize() const
        {   return sizeof(m_acWiFiSSID); }

        //! Returns WiFi Passphrase. Null terminated.
        const char* GetWiFiPass() const
        {   return m_acWiFiPass; }

        //! Returns WiFi Passphrase. Null terminated.
        char* GetWiFiPass()
        {   return m_acWiFiPass; }

        //! Returns WiFi Passphrase size.
        uint16_t GetWiFiPassSize() const
        {   return sizeof(m_acWiFiPass); }

protected:
        //! The only static singleton object.
        static Settings* s_pInstance;

        //! Device serial number
        uint32_t m_uiSerialNumber;

        //! UDP local listen port
        uint16_t m_uiUdpListenPort = 0;

        //! UDP remote port for sending datagrams
        uint16_t m_uiUdpRemotePort = 0;

        //! UDP remote IP for sending datagrams
        uint32_t m_uiUdpRemoteIP = 0;

        //! Access point SSID to connect to. Null terminated.
        char     m_acWiFiSSID[33];

        //! Access point Passphrase. Null terminated.
        char     m_acWiFiPass[33];

        //! Hold settings in the Flash
        Preferences m_preferences;
};

#endif // SETTINGS_H
