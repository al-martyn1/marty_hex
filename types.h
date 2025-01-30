/*! \file
    \brief Type definitions for marty_hex
 */

#pragma once

//----------------------------------------------------------------------------
#include "utils.h"
//
#include <string>
#include <vector>
#include <utility>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// marty_hex/types.h
// marty::hex::
namespace marty{
namespace hex{

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Для отладки используем вектор - в нём отладчик нормас отображает значения произвольных целых типов в векторе (особенно в MSVC)
// Для релиза используем строку - у ней есть SSO, не лазает постоянно в память, короткие байтовые последовательности в ней хранить сам доктор прописал
#if defined(_DEBUG) || defined(DEBUG)
    using byte_vector = std::vector<std::uint8_t>;
#else
    using byte_vector = std::basic_string<std::uint8_t>;
#endif

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
void intVectorEraseHelper( std::basic_string<std::uint8_t> &vec, std::size_t offs, std::size_t sz)
{
    vec.erase(offs, sz);
}

//------------------------------
inline
void intVectorEraseHelper( std::vector<std::uint8_t> &vec, std::size_t offs, std::size_t sz)
{
    vec.erase(vec.begin()+std::ptrdiff_t(offs), vec.begin()+std::ptrdiff_t(offs+sz));
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
void intVectorAppendHelper( std::basic_string<std::uint8_t> &vec, std::uint8_t b)
{
    vec.append(1, b);
}

//------------------------------
inline
void intVectorAppendHelper( std::vector<std::uint8_t> &vec, std::uint8_t b)
{
    vec.emplace_back(b);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/types.h

