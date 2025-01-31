/*! \file
    \brief Utils for hex file parsing
 */

#pragma once

#include "enums.h"

#include <cstdint>
#include <iterator>
#include <string>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
namespace marty{
namespace hex{
namespace utils {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
int charToDigit(char ch)
{
    if (ch>='0' && ch<='9')
        return ch-'0';
    if (ch>='A' && ch<='Z')
        return ch-'A'+10;
    if (ch>='a' && ch<='z')
        return ch-'a'+10;
    return -1;
}

//----------------------------------------------------------------------------
inline
char digitToChar(int d, bool bLower=false)
{
    d &= 0xF;
    // if (d<0)  return '-';
    if (d<10) return char('0'+d);
    return char((bLower?'a':'A')+d-10);
}

//----------------------------------------------------------------------------
template<typename OutputIterator>
OutputIterator byteToHex(std::uint8_t b, OutputIterator oit, bool bLower=false)
{
    *oit++ = digitToChar(b>>4, bLower);
    *oit++ = digitToChar(b   , bLower);
    return oit;
}

template<typename OutputIterator>
OutputIterator address16ToHex(std::uint16_t a, OutputIterator oit, bool bLower=false)
{
    oit = byteToHex(std::uint8_t(a>>8 ), oit, bLower);
    oit = byteToHex(std::uint8_t(a    ), oit, bLower);
    return oit;
}

template<typename OutputIterator>
OutputIterator address32ToHex(std::uint32_t a, OutputIterator oit, bool bLower=false)
{
    oit = byteToHex(std::uint8_t(a>>24), oit, bLower);
    oit = byteToHex(std::uint8_t(a>>16), oit, bLower);
    oit = byteToHex(std::uint8_t(a>>8 ), oit, bLower);
    oit = byteToHex(std::uint8_t(a    ), oit, bLower);
    return oit;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
std::string address16ToString(std::uint16_t a)
{
    std::string str;
    address16ToHex(a, std::back_inserter(str));
    return str;
}

inline
std::string address32ToString(std::uint32_t a, AddressMode addressMode=AddressMode::lba)
{
    std::string str;
    switch(addressMode)
    {
        case AddressMode::lba:
             address32ToHex(a, std::back_inserter(str));
             break;

        case AddressMode::sba:
             str = address16ToString(std::uint16_t(a>>16)) + ":" + address16ToString(std::uint16_t(a));
             break;

        case AddressMode::none:
             str = "--------";
             break;

        default:
             return "XXXXXXXX";
    }

    return str;
}

inline 
std::string addressModeToString(AddressMode addressMode, bool bShort)
{
    switch(addressMode)
    {
        case AddressMode::lba:
             return bShort ? "LBA" : "Linear Base Address";

        case AddressMode::sba:
             return bShort ? "SBA" : "Segment Base Address";

        case AddressMode::none:
             return bShort ? "NONE" : "Not set";

        default:
             return "Unknown";
    }
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define MARTY_HEX_OUTPUT_ITERATOR_BILLET(cls)                     \
            cls& operator++()   { return *this; }                 \
            cls operator++(int) { return *this; }                 \
                                                                  \
            /* Simply returns *this (See back_insert_iterator     \
               implementation in GCC c++/bits/stl_iterator.h) */  \
            cls& operator*()    { return *this; }                 \
                                                                  \
            cls() = delete;                                       \
            cls(const cls&) = default;                            \
            cls(cls&&) = default;                                 \
            cls& operator=(const cls&) = default;                 \
            cls& operator=(cls&&) = default


//----------------------------------------------------------------------------
class HexOutputIterator
{
    std::string   *m_pStr;
    std::uint8_t  *m_pChecksum;

public:

    MARTY_HEX_OUTPUT_ITERATOR_BILLET(HexOutputIterator);

    explicit HexOutputIterator(std::string &str, std::uint8_t &checksum) : m_pStr(&str), m_pChecksum(&checksum) {}

    HexOutputIterator& operator=(std::uint8_t b)
    {
        *m_pChecksum += b;
        m_pStr->append(1, digitToChar(b>>4));
        m_pStr->append(1, digitToChar(b));
        return *this;
    }


}; // HexOutputIterator





//----------------------------------------------------------------------------

} // namespace utils
} // namespace hex
} // namespace marty

