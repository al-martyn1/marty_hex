/*! \file
    \brief Hex file parsing (Intel HEX)
 */

#pragma once

#include "enums.h"

#include <string>
#include <cstdint>


namespace marty{
namespace hex{

namespace utils {

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

} // namespace utils


} // namespace hex
} // namespace marty

