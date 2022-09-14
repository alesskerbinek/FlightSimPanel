#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <stddef.h>
#include <Arduino.h>

// -------------------------------------------------------------------------
//! Used to determine number of elements for static arrays.
// -------------------------------------------------------------------------

namespace helper {

//-----------------------------------------------------------------------------

inline uint32_t GetTime()
{ return millis(); }

//-----------------------------------------------------------------------------

//! Get time difference [ms] between old measurement and given time.
inline int32_t GetTimeDifference(uint32_t uiOldTime, uint32_t uiNewTime)
{ return (int32_t)uiNewTime - (int32_t)uiOldTime; }

//-----------------------------------------------------------------------------

//! Get time difference [ms] between old measurement and current.
inline int32_t GetTimeDifference(uint32_t uiOldTime)
{ return GetTimeDifference(uiOldTime, GetTime()); }

//-----------------------------------------------------------------------------

} //namespace

#endif // HELPER_H
