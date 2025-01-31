/*! \file
    \brief File position info
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
struct FilePosInfo
{
    std::size_t line = 0;
    std::size_t pos  = 0;
    std::size_t file = std::size_t(-1); // id

}; // struct FilePosInfo

//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/marty_hex.h



