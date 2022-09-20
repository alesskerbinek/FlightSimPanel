#ifndef TYPESDEF_H
#define TYPESDEF_H

#include <stdint.h>
#include <stddef.h>
#include <cstring>

namespace types {

// --------------------------------------------------------------------

struct SystemParameters
{
    SystemParameters() {
        Clear();
    }

    SystemParameters(
            uint32_t uiLoopCount)
    {
        m_uiLoopCount = uiLoopCount;
    }

    void Clear() {
        m_uiLoopCount = 0;
    }

    uint32_t    m_uiLoopCount;
};

// --------------------------------------------------------------------

struct ProductionParameters
{
    ProductionParameters() {
        Clear();
    }

    ProductionParameters(
            int32_t iSerialNumber,
            int8_t  iType)
    {
        m_iSerialNumber = iSerialNumber;
        m_iType = iType;
    }

    void Clear() {
        m_iSerialNumber = 0;
        m_iType = 0;
    }

    int32_t    m_iSerialNumber;
    int8_t     m_iType;
};

// --------------------------------------------------------------------

struct NetworkParameters
{
    NetworkParameters() {
        Clear();
    }

    NetworkParameters(
            const char* acWiFiSSID,
            const char* acWiFiPass,
            int32_t  iUdpListenPort, // -1 = invalid value
            int32_t  iUdpRemotePort, // -1 = invalid value
            uint32_t uiUdpRemoteIP)  //  0 = invalid value
    {
        strncpy(m_acWiFiSSID, acWiFiSSID, sizeof(m_acWiFiSSID));
        strncpy(m_acWiFiPass, acWiFiPass, sizeof(m_acWiFiPass));
        m_iUdpListenPort = iUdpListenPort;
        m_iUdpRemotePort = iUdpRemotePort;
        m_uiUdpRemoteIP = uiUdpRemoteIP;
    }

    void Clear() {
        m_acWiFiSSID[0] = '\0';
        m_acWiFiPass[0] = '\0';
        m_iUdpListenPort = 0;
        m_iUdpRemotePort = 0;
        m_uiUdpRemoteIP = 0;
    }

    char     m_acWiFiSSID[33];
    char     m_acWiFiPass[33];
    int32_t  m_iUdpListenPort;
    int32_t  m_iUdpRemotePort;
    uint32_t m_uiUdpRemoteIP;
};

// --------------------------------------------------------------------


} //namespace

#endif // TYPESDEF_H
