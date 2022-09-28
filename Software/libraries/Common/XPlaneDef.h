#ifndef XPLANEDEF_H
#define XPLANEDEF_H

namespace xplane {

// --------------------------------------------------------------------

/**
 * @brief Udp message types
 */
enum UdpDataType {
    dtUndefined = 0,
    dtDATA,
    dtDREF,
    dtCHAR,
    dtCMND,
    dtCount
};

// --------------------------------------------------------------------

/**
 * @brief The DATA message ids that are being parsed
 */
enum DataId {
    idComFreq = 96,
    idNavFreq,
    idNavObs,
};

// --------------------------------------------------------------------

/**
 * @brief DataRefs that are being sent to X-Plane simulator
 */
enum DataRefs {
    drUndefined = 0,
    drCom1Freq,
};

// --------------------------------------------------------------------

/**
 * @brief Commands that are being sent to X-Plane simulator
 */
enum Commands {
    cmUndefined = 0,
    cmLandingGear,
};

// --------------------------------------------------------------------

/**
 * @brief Udp datatgram structure used for Tx queue.
 * It does not contain whole datagram bytes but enums for individual
 * commands/datarefs to save some memory.
 */
struct UdpDatagram {
    UdpDatagram(UdpDataType eTyp, uint32_t uiPar, uint32_t uiVal = 0) {
        m_eType = eTyp;
        m_uiValue = uiVal;
        m_uiParameter = uiPar;
    }

    UdpDatagram(UdpDataType eTyp, uint32_t uiPar, float fVal) {
        m_eType = eTyp;
        m_uiParameter = uiPar;
        memcpy(&m_uiValue, &fVal, sizeof(m_uiValue));
    }

    UdpDataType m_eType;
    uint32_t m_uiParameter;
    uint32_t m_uiValue;
};

// --------------------------------------------------------------------

} //namespace

#endif // XPLANEDEF_H