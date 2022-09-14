#ifndef CONFIGPORT_H_
#define CONFIGPORT_H_

#include <stdint.h>
#include <stddef.h>
#include <array>
#include "TypesDef.h"

#define MAX_NMEA_SIZE					82-2-2-1 // 82 is max size by NMEA0183 minus * CRC <CR><LF>
#define MAX_NMEA_FIELDS					24
#define CR_BYTE							0x0D
#define LF_BYTE							0x0A

class ConfigPort
{
public:
    //! NMEA sentence identifiers.
    enum class Sentence : uint8_t {
        sInvalid = 0,
        sPMDBG,
        sPMERR,
        sPINIT,
        sPSYS0,
        sPFACT
    };

protected:
    //! Process (parsing) state.
    enum ProcessState {
        psWaitStart,	//!< Waiting for $
        psReadMsg,		//!< Reading body
        psReadCRC1,		//!< Read first CRC byte.
        psReadCRC2,		//!< Read second CRC byte.
        psSuccess,		//!< Final, ready to parse.
        psWaitTail,		//!< Waiting for tail.
        psError			//!< Indicates an error.
    };

public:
	//! Constructor
    ConfigPort();
	
	//! Destructor
    virtual ~ConfigPort() {;}

    //! Initialization of members not available during construction.
    virtual void Initialize();

    //! Main state machine loop.
    virtual void Process();

    //! Static send function which works directly on the bus.
    bool Send(const char* pString, bool bAddCRC = true);
	
protected:
    //! Process byte received
    void ProcessByte(uint8_t byte);

    //! Parse byte array received
    virtual void Parse();

    //! Parse NMEA sentence PINIT
    bool ParsePINIT();

    //! Parse NMEA sentence PSYSx
    bool ParsePSYSx(uint16_t uiId);

    //! Parse NMEA sentence PFACT
    bool ParsePFACT();

    //! Get preprocessed field - fast access.
    const char* GetField(int i) const
    { return (i <= m_byFieldCount)? &m_acReceive[m_abyField[i]] : ""; }

    //! Returns serial number, type etc. Reimplement in inherited class.
    virtual void GetProductionParameters(types::ProductionParameters* sParam) = 0;

    //! Sets serial number, type etc. Reimplement in inherited class.
    virtual bool SetProductionParameters(types::ProductionParameters* sParam) = 0;

    //! Returns system parameters. Reimplement in inherited class.
    virtual void GetSystemParameters(types::SystemParameters* sParam) = 0;

    //! Performs Factory Reset. Reimplement in inherited class.
    virtual bool FactoryReset() = 0;

protected:
    //! Receive buffer.
    std::array<char, MAX_NMEA_SIZE + 20> m_acReceive;
    //! Receive buffer cursor position.
    int8_t m_iPos;
    //! CRC byte - calculated on the fly.
    uint8_t m_byCalculatedCRC;
    //! CRC byte - read from bus.
    uint8_t m_byReadCRC;
    //! Number of fields.
    uint8_t m_byFieldCount;
    //! Fields - for faster parsing.
    std::array<uint8_t, MAX_NMEA_FIELDS+4> m_abyField;
    //! State of the process (parser).
    ProcessState m_eState;
    //! Successfully parsed nmea sentences.
    uint32_t m_uiSentenceCount;
    //! Get sentence processed during the last Process call.
    Sentence m_nmeaSentence;


};

#endif /* CONFIG_H_ */
