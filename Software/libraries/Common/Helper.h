#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <stddef.h>
#include <Arduino.h>

#define HEXCHARS	"0123456789abcdef"

#define IsValidDigit(c) ((c)>='0' && (c)<='9')

namespace helper {

//-----------------------------------------------------------------------------

/**
* Function returning the time in microseconds since system boot.
* The value overflows in approx. 70 minutes.
* @return microseconds seconds since boot
*/
inline uint32_t GetTimeUs()
{ return micros(); }


//-----------------------------------------------------------------------------

/**
* Function returning the time in miliseconds since system boot.
* @return miliseconds since boot
*/
inline uint32_t GetTime()
{ return millis(); }

//-----------------------------------------------------------------------------

/**
* Function returning the difference between given times. The time unit can
* be anything but has to be the same for both parameters.
* @param First time in time units
* @param Second time in time units
* @return The difference in time units
*/
inline int32_t GetTimeDifference(uint32_t uiOldTime, uint32_t uiNewTime)
{ return (int32_t)uiNewTime - (int32_t)uiOldTime; }

//-----------------------------------------------------------------------------

/**
* Function returning the difference between given time and current moment.
* @param Time in miliseconds
* @return The difference in miliseconds
*/
inline int32_t GetTimeDifference(uint32_t uiOldTime)
{ return GetTimeDifference(uiOldTime, GetTime()); }

// ---------------------------------------------------------------------


/**
* Function returning numeric value from given character.
* @param ASCII character (0-F)
* @return numeric value (0-15)
*/
inline unsigned char GetByteFromAscii(char byAscii)
{
    if(byAscii >= '0' && byAscii <= '9')
        return byAscii - '0';
    else if(byAscii >= 'A' && byAscii <= 'F')
        return byAscii - 'A' + 10;
    else if(byAscii >= 'a' && byAscii <= 'f')
        return byAscii - 'a' + 10;
    else
        return 0xFF;
}

// ---------------------------------------------------------------------

/**
* Function returning ASCII character from given numeric value.
* @param Numeric value (0-15)
* @return ASCII character (0-F)
*/
inline char GetAsciiFromByte(uint8_t by)
{
    return HEXCHARS[by];
}

// ---------------------------------------------------------------------

/**
* Function returning integer value from given character array.
* Leading spaces are ignored, after that the sign character (+, -) or valid
* numeric value has to follow. Possible additional non-numeric characters
* at the end are ignored.
* @param ASCII character encoded integer value.
* @param Default value in case conversion fails.
* @return Numeric value.
*/
inline int StrToInt(const char* pB, int iDefault = 0)
{
    while(isspace(*pB))
        pB++;

    if(*pB == 0)
        return iDefault;

    bool bNeg = false;
    if(*pB=='-') {
        bNeg = true;
        pB++;
    }
    else if(*pB=='+') {
        pB++;
    }

    int iDec = 0;
    bool bOK = false;
    while(IsValidDigit(*pB)) {
        iDec = iDec*10 + (*pB++ - '0');
        bOK = true;
    }

    if (!bOK)
        return iDefault;

    if(bNeg)
        iDec = -iDec;

    return iDec;
}

// ---------------------------------------------------------------------

/**
* Function returning 32-bit integer value from given character array encoded
* IP address.
* The character array has to be in "x.x.x.x" format, where x is integer between
* 0 and 255. In case the array is in wrong format or the conversion can not be
* performed from any reason, value 0 is returned.
* @param ASCII character encoded IP address.
* @return Numeric value.
*/
inline uint32_t IPtoInt(const char* pSource)
{
    char acBuffer[16];
    memcpy(acBuffer, pSource, sizeof(acBuffer));
    uint32_t uiValue = 0;
    uint8_t uiNextSegment = 0;
    uint8_t uiCount = 0;
    for(uint8_t i=0; i<sizeof(acBuffer); i++) {
        bool bBreakAfter = acBuffer[i] == '\0';
        if(acBuffer[i] == '.' || bBreakAfter) {
            int v = strtoul(&acBuffer[uiNextSegment], NULL, 10);
            uiValue += (v << ((3-uiCount)*8));
            uiNextSegment=i+1;
            acBuffer[i] = '\0';
            uiCount++;
            if(bBreakAfter) {
                break;
            }
        }
    }
    if(uiCount != 4) {
        return 0;
    } else {
        return uiValue;
    }
}

// ---------------------------------------------------------------------

/**
* Function returning IPAddress from given 4 byte integer value.
* The MSB is the first quadrant of IP address and LSB the last.
* @param Integer value.
* @return IPAdress object.
*/
inline IPAddress GetIPAddress(uint32_t uiValue)
{
    return IPAddress((uiValue >> 24) & 0xFF, (uiValue >> 16) & 0xFF,
                    (uiValue >> 8) & 0xFF, uiValue & 0xFF);
}

// --------------------------------------------------------------------

/**
* Function tests the given value to fit inside the given limits.
* In case value is below the minimum limit, minimum limit is returned.
* In case value is above the maximum limit, maximum limit is returned.
* In case given value is between the limits, given value is returned.
* @param Value to test
* @param Minimum limit
* @param Maximum limit
* @return Value between the limits.
*/
template<class T>
inline T GetMinMax(const T& x, const T& min, const T& max)
{
    if(x < min)
        return min;
    else if(x > max)
        return max;
    else
        return x;
}

// --------------------------------------------------------------------

} //namespace

#endif // HELPER_H
