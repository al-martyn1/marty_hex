/*! \file
    \brief BitVector class
 */

#pragma once

//----------------------------------------------------------------------------
#include "utils.h"
#include "bit_vector.h"
//
#include <iterator>
#include <map>
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
class MemoryFillMap
{

public:

    using address_t = std::uint32_t;

protected:

    std::map<address_t, BitVector<address_t> >    m_fillMap;


public:

    MemoryFillMap() = default;
    MemoryFillMap(const MemoryFillMap &) = default;
    MemoryFillMap(MemoryFillMap &&) = default;
    MemoryFillMap& operator=(const MemoryFillMap &) = default;
    MemoryFillMap& operator=(MemoryFillMap &&) = default;


    bool getFilled(address_t byteAddr) const
    {
        address_t base = byteAddr&~0xFFFFu;
        std::map<address_t, BitVector<address_t> >::const_iterator it = m_fillMap.find(base);
        if (it==m_fillMap.end())
            return false;

        return it->second.getBit(byteAddr&0xFFFFu);
    }

    void setFilled(address_t byteAddr, bool bVal=true)
    {
        address_t base = byteAddr&~0xFFFFu;
        auto &bv = m_fillMap[base];
        bv.setBit(byteAddr&0xFFFFu, bVal);
    }

    template<typename StreamType>
    StreamType& operator<<(StreamType &oss) const
    {
        if (m_fillMap.empty())
        {
            oss << "<EMPTY>\n";
            return oss;
        }

        address_t lastChunkEndAddr = 0;
        std::map<address_t, BitVector<address_t> >::const_iterator it = m_fillMap.begin();
        for(; it!=m_fillMap.end(); ++it)
        {
            if (it!=m_fillMap.begin() && it->first!=lastChunkEndAddr)
            {
                oss << "...\n";
            }

            address_t byteIdx = 0;
            for(; byteIdx!=it->second.size(); ++byteIdx)
            {
                if ((byteIdx%64)==0)
                {
                    if (byteIdx)
                        oss << "\n";
                    auto addr = it->first+byteIdx;
                    std::string strAddr;
                    utils::address32ToHex(addr, std::back_inserter(strAddr));
                    oss << strAddr << " : ";
                }

                oss << (it->second.getBit(byteIdx)?"X":"-");
            }
            
            bool isSizeMultiple64 = it->second.size() % 64u;
            lastChunkEndAddr = it->second.size() / 64u;
            if (isSizeMultiple64)
                ++lastChunkEndAddr;
            // else // Короткая строка?

            lastChunkEndAddr *= 64u;

            oss << "\n"; 
        }

        return oss;

    }



    // bool getBit(bit_index_t bitIndex) const
    // void setBit(bit_index_t bitIndex, bool bVal)


// typename<typename BitIndexType>
// class BitVector
// {


}; // class MemoryFillMap


// OutputIterator byteToHex(std::uint8_t b, OutputIterator oit, bool bLower=false)
// OutputIterator address32ToHex(std::uint32_t a, OutputIterator oit, bool bLower=false)



//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/types.h

