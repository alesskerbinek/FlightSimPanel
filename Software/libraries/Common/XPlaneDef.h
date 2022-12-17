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
    idComFreq = 96,                 // 25kHz only
    idNavFreq,                      // OK
    idNavObs,
    idAdfStatus = 101,
    idDmeStatus,
    idXpndrStatus = 104,
    idMarkerStatus,
    idElectricalSwitches,
    idAutopilotStatus = 108,
    idGeneralAnnunicators1 = 113,
    idGeneralAnnunicators2,
    idEngineAnnunicators,
    idAutopilotArmedStatus,
    idAutopilotModes,
    idAutopilotValues,
    idElectircalAndSolar = 124,
    idHardwareOptions = 129,
};

// --------------------------------------------------------------------

/**
 * @brief DataRefs that are being sent to X-Plane simulator
 */
enum DataRefs {
    drUndefined = 0,
    drNav1StandbyHz,
    drNav1StandbyMHz,
    drNav1StandbykHz,
    drCom1ActiveFreq,
    drCom1StandbyFreq,
    drCom2ActiveFreq,
    drCom2StandbyFreq,
    drCom1Power,
    drXpdrCode,
};

// --------------------------------------------------------------------

/**
 * @brief Commands that are being sent to X-Plane simulator
 */
enum Commands {
    cmUndefined = 0,
    cmNav1Flip,
    cmNav2Flip,
    cmCom1Flip,
    cmCom2Flip,
    cmAdf1Flip,
    cmAdf2Flip,
    cmDmeFlip,
    cmNav1StandbyCoarseUp,
    cmNav1StandbyCoarseDown,
    cmNav1StandbyFineUp,
    cmNav1StandbyFineDown,
    cmNav2StandbyCoarseUp,
    cmNav2StandbyCoarseDown,
    cmNav2StandbyFineUp,
    cmNav2StandbyFineDown,
    cmCom1StandbyCoarseUp,
    cmCom1StandbyCoarseDown,
    cmCom1StandbyFineUp,
    cmCom1StandbyFineDown,
    cmCom1Standby833Up,
    cmCom1Standby833Down,
    cmCom2StandbyCoarseUp,
    cmCom2StandbyCoarseDown,
    cmCom2StandbyFineUp,
    cmCom2StandbyFineDown,
    cmCom2Standby833Up,
    cmCom2Standby833Down,
    cmCom1On,
    cmCom1Off,
    cmCom2On,
    cmCom2Off,
    cmNav1ObsUp,
    cmNav1ObsDown,
    cmNav2ObsUp,
    cmNav2ObsDown,
    cmXpdrX000Up,
    cmXpdrX000Down,
    cmXpdr0X00Up,
    cmXpdr0X00Down,
    cmXpdr00X0Up,
    cmXpdr00X0Down,
    cmXpdr000XUp,
    cmXpdr000XDown,
    cmXpdrIdent,
    cmXpdrOff,
    cmXpdrSby,
    cmXpdrOn,
    cmXpdrAlt,
    cmLandingGear,
    cmFlapsUp,
    cmFlapsDown,
};

// --------------------------------------------------------------------

/**
 * @brief Transponder modes
 */
enum XpdrModes {
    xmOff = 0,
    xmSby,
    xmOn,
    xmAlt,
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
