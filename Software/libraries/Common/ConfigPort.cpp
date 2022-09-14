#include "Helper.h"
#include "ConfigPort.h"

//-----------------------------------------------------------------------------

ConfigPort::ConfigPort()
{
    m_nmeaSentence = Sentence::sInvalid;
    m_eState = psWaitStart;
    m_uiSentenceCount = 0;
    m_byReadCRC = 0;
    m_byFieldCount = 0;
    m_byCalculatedCRC = 0;
    m_iPos = 0;
    m_acReceive.fill(0);
    m_abyField.fill(0);
}

// ----------------------------------------------------------------------------

void ConfigPort::Initialize()
{

}

//-----------------------------------------------------------------------------

void ConfigPort::Process()
{

}

//-----------------------------------------------------------------------------

bool ConfigPort::Send(
    const char* pString,
    bool bAddCRC)
{
    // String must be NULL terminated without leading $
    int iN = strlen(pString);

    // Transmit the the leading $
    Serial.write('$');
    // Calculate the crc.
    uint8_t byCRC = 0;
    for (int32_t i = 0; i < iN; i++) {
        byCRC ^= pString[i];
    }
    // Now, transmit the string
    if (Serial.write((uint8_t*)pString, iN) == iN) {
        if (bAddCRC) {
            // Transmit the the CRC
            Serial.write('*');
            // High byte
            Serial.write(helper::GetAsciiFromByte(byCRC >> 4));
            // Low byte
            Serial.write(helper::GetAsciiFromByte(byCRC & 0x0F));
        }
        Serial.write(CR_BYTE);
        Serial.write(LF_BYTE);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------

void ConfigPort::ProcessByte(uint8_t byte)
{
    std::array<char, 150> acBuff;

    switch(m_eState) {

    // Wait for the '$' to appear.
    case psWaitStart:
        if (byte == '$') {
            m_acReceive.fill(0);
            m_abyField.fill(0);
            m_acReceive.at(0) = byte;
            m_iPos = 1;
            m_byCalculatedCRC = 0;
            m_byFieldCount = 0;
            m_abyField.at(0) = 0;
            m_eState = psReadMsg;
        }
        break;

    // Are we reading the message?
    case psReadMsg:
        // Reset parser
        if (byte == '$') {
            m_acReceive.fill(0);
            m_abyField.fill(0);
            m_acReceive.at(0) = byte;
            m_iPos = 1;
            m_byCalculatedCRC = 0;
            m_byFieldCount = 0;
            m_abyField.at(0) = 0;
            m_eState = psReadMsg;
        }
        // Check for the CRC mark *
        else if (byte == '*') {
            m_byFieldCount++;
            m_acReceive.at(m_iPos) = 0;
            m_eState = psReadCRC1;
        }
        // Normal character in the message
        else {
            // Update the CRC
            m_byCalculatedCRC ^= byte;
            // check for the comma and make a replacement for the parser.
            if (byte == ',') {
                byte = 0;
                m_byFieldCount++;
                m_abyField.at(m_byFieldCount) = m_iPos + 1;
            }
            m_acReceive.at(m_iPos) = byte;
            m_iPos++;
            // Prevent too many fields
            if (m_byFieldCount >= MAX_NMEA_FIELDS) {
                Send(static_cast<const char*>("PMERR,NMEA,field count error - max 24"), false);
                m_eState = psWaitStart;
            }
            // Prevent too long messages...
            if (m_iPos >= MAX_NMEA_SIZE) {
                Send(static_cast<const char*>("PMERR,NMEA,size error - max 82"), false);
                m_eState = psWaitStart;
            }
        }
        break;

    // Get first CRC character
    case psReadCRC1:
        m_byReadCRC = helper::GetByteFromAscii(byte)*16;
        m_eState = psReadCRC2;
        break;

    // Get second CRC character and calculate the match
    case psReadCRC2:
        m_byReadCRC += helper::GetByteFromAscii(byte);
        // Success!
        if(m_byReadCRC == m_byCalculatedCRC) {
            // Increment successfully received sentence count.
            m_uiSentenceCount++;
            // Call the parser to process the sentence in the buffer.
            Parse();
        }
        // CRC Error
        else {
            snprintf(acBuff.data(), acBuff.size(), "PMERR,CRC,%u,%u", m_byReadCRC, m_byCalculatedCRC);
            Send(static_cast<const char*>(acBuff.data()), false);
        }
        // We wait for the tail byte.
        m_eState = psWaitStart;
        break;

    default:
        // Should not end here
        break;
    }
}

//-----------------------------------------------------------------------------

void ConfigPort::Parse()
{
    const char* pcData = static_cast<const char*>(m_acReceive.data());
    // Skip $
    pcData++;
    // Check for sentence
    if (strncmp(pcData, "PINIT", 5) == 0) {
        if (ParsePINIT() == true) {
            m_nmeaSentence = Sentence::sPINIT;
            return;
        }
    } else if (strncmp(pcData, "PSYS0", 5) == 0) {
        if (ParsePSYSx(0) == true) {
            m_nmeaSentence = Sentence::sPSYS0;
            return;
        }
    } else if (strncmp(pcData, "PFACT", 5) == 0) {
        if (ParsePFACT() == true) {
            m_nmeaSentence = Sentence::sPFACT;
            return;
        }
    }
}

//-----------------------------------------------------------------------------

bool ConfigPort::ParsePINIT()
{
    const char* pB = GetField(1);

    if(strncmp(pB, "GET", 3) == 0) {
        types::ProductionParameters sParam;
        GetProductionParameters(&sParam);
        std::array<char, MAX_NMEA_SIZE-1> acBuff;
        snprintf(acBuff.data(), acBuff.size(), "PINIT,SET,%ld,%d", sParam.m_iSerialNumber, sParam.m_iType);
        Send(static_cast<const char*>(acBuff.data()), true);
        return true;
    }
    else if(strncmp(pB, "SET", 3) == 0){
        // Parse parameters
        int32_t iSerialNum = helper::StrToInt(GetField(2), -1);
        int8_t  iType = helper::StrToInt(GetField(3), -1);
        // Write parameters in memory
        types::ProductionParameters sParam = types::ProductionParameters(iSerialNum, iType);
        if (SetProductionParameters(&sParam)) {
            Send("PMACK,OK", true);
            return true;
        }
    }
    Send("PMACK,ERROR,", true);
    return false;
}

//-----------------------------------------------------------------------------

bool ConfigPort::ParsePSYSx(uint16_t uiId)
{
    const char* pB = GetField(1);

    if(strncmp(pB, "GET", 3) == 0) {
        switch (uiId) {
        case 0: {
            types::SystemParameters sParam;
            GetSystemParameters(&sParam);
            std::array<char, MAX_NMEA_SIZE-1> acBuff; // minus $
            snprintf(acBuff.data(), acBuff.size(), "PSYS0,SET,%lu,%lu,%lu,%lu", sParam.m_uiLoopCount, ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getMaxAllocHeap());
            Send(static_cast<const char*>(acBuff.data()), true);
            return true;
        }
        default:
            break;
        }
    }
    Send("PMACK,ERROR,", true);
    return false;
}

//-----------------------------------------------------------------------------

bool ConfigPort::ParsePFACT()
{
    const char* pB = GetField(1);

    if(strncmp(pB, "SET", 3) == 0){
        // Parse parameters
        if (FactoryReset()) {
            Send("PMACK,OK", true);
            return true;
        }
    }
    Send("PMACK,ERROR,", true);
    return false;
}

//-----------------------------------------------------------------------------
