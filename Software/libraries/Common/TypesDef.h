#ifndef TYPESDEF_H
#define TYPESDEF_H

#include <stdint.h>
#include <stddef.h>

// --------------------------------------------------------------------

namespace types {

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

} //namespace

#endif // TYPESDEF_H
