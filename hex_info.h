/*! \file
    \brief Hex block/file info
 */

#pragma once

//----------------------------------------------------------------------------
#include <cstdint>

//----------------------------------------------------------------------------


// marty_hex/marty_hex.h
// marty::hex::
namespace marty{
namespace hex{

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct HexInfo
{
    std::uint32_t baseAddress  = std::uint32_t(-1);
    std::uint32_t startAddress = std::uint32_t(-1);
    AddressMode   addressMode  = AddressMode::none;

}; // struct HexInfo
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/marty_hex.h

