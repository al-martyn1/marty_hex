/*! \file
    \brief Utils for hex file parsing
 */

#pragma once

#include "enums.h"

#include <string>
#include <cstdint>

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
    if (d<10) return '0'+d;
    return (bLower?'a':'A')+d-10;
}

//----------------------------------------------------------------------------
template<typename OutputIterator>
OutputIterator byteToHex(std::uint8_t b, OutputIterator oit, bool bLower=false)
{
    *oit++ = digitToChar(b>>4);
    *oit++ = digitToChar(b);
    return oit;
}



class HexOutputIterator
{
    std::string   &m_str;
    std::uint8_t  &m_checksum;

public:


    explicit HexOutputIterator(std::string &str, std::uint8_t  &checksum) : m_str(str), m_checksum(checksum) {}
    HexOutputIterator() = delete;
    HexOutputIterator(const HexOutputIterator&) = default;
    HexOutputIterator(HexOutputIterator&&) = default;
    HexOutputIterator& operator=(const HexOutputIterator&) = default;
    HexOutputIterator& operator=(HexOutputIterator&&) = default;


    HexOutputIterator& operator++()   { return *this; }
    HexOutputIterator operator++(int) { return *this; }

    // Simply returns *this (See back_insert_iterator implementation in GCC c++/bits/stl_iterator.h)
    HexOutputIterator& operator*()    { return *this; }

    HexOutputIterator& operator=(std::uint8_t b)
    {
        m_checksum += b;
        m_str.append(1, digitToChar(b>>4));
        m_str.append(1, digitToChar(b));
        return *this;
    }


}; // HexOutputIterator



//----------------------------------------------------------------------------

} // namespace utils
} // namespace hex
} // namespace marty

