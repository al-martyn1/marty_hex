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

    using address_t      = std::uint32_t;
    using bit_vector_t   = BitVector<address_t>;
    using memory_range_t = typename bit_vector_t::bit_index_range_t;


protected:

    std::map<address_t, bit_vector_t >    m_fillMap;


public:

    static
    inline bool ostreamWideOutput = true; 

    MemoryFillMap() = default;
    MemoryFillMap(const MemoryFillMap &) = default;
    MemoryFillMap(MemoryFillMap &&) = default;
    MemoryFillMap& operator=(const MemoryFillMap &) = default;
    MemoryFillMap& operator=(MemoryFillMap &&) = default;

    // TODO: Хорошо бы сделать кеш на чтение и на запись - хранить итератор, по которому было
    // последнее обращение - при обращении к одной и той же "странице" будет исключаться поиск в map,
    // а он логарифмический. Хотя, обычно прошивки от силы занимают несколько страниц, но всё равно,
	    // на несколько сотен килобайт для каждого байта производить поиск - дорого


    bool getFilled(address_t byteAddr) const
    {
        address_t base = byteAddr&~0xFFFFu;
        std::map<address_t, bit_vector_t >::const_iterator it = m_fillMap.find(base);
        if (it==m_fillMap.end())
            return false;

        return it->second.getBit(byteAddr&0xFFFFu);
    }

    void setFilled(address_t byteAddr, bool bVal=true)
    {
        address_t base   = byteAddr&~0xFFFFu;
        address_t offset = byteAddr& 0xFFFFu;
        auto &bv = m_fillMap[base];
        bv.setBit(offset, bVal);
    }

    template<typename StreamType>
    StreamType& printTo(StreamType &oss, bool bWide) const
    {
        if (m_fillMap.empty())
        {
            oss << "<EMPTY>\n";
            return oss;
        }

        const address_t lineWidth = bWide ? 128u : 64u;

        address_t lastChunkEndAddr = 0;
        std::map<address_t, bit_vector_t >::const_iterator it = m_fillMap.begin();
        for(; it!=m_fillMap.end(); ++it)
        {
            if (it!=m_fillMap.begin() && it->first!=lastChunkEndAddr)
            {
                oss << "...\n";
            }

            address_t byteIdx = 0;
            for(; byteIdx!=it->second.size(); ++byteIdx)
            {
                if ((byteIdx%lineWidth)==0)
                {
                    if (byteIdx)
                        oss << "\n";
                    auto addr = it->first+byteIdx;
                    std::string strAddr;
                    utils::address32ToHex(addr, std::back_inserter(strAddr));
                    oss << strAddr << " : ";
                }
                else if ((byteIdx%16)==0)
                {
                    if (byteIdx)
                        oss << " ";
                }

                oss << (it->second.getBit(byteIdx)?"X":"-");
            }
            
            lastChunkEndAddr = it->first + it->second.size(); // + 1;
            // bool isSizeMultipleWidth = (it->second.size() % lineWidth)==0;
            // lastChunkEndAddr = it->second.size() / lineWidth;
            // if (!isSizeMultipleWidth)
            //     ++lastChunkEndAddr;
            // // else // Короткая строка?
            //  
            // lastChunkEndAddr *= lineWidth;
            // ++lastChunkEndAddr;

            oss << "\n"; 
        }

        return oss;

    }

    std::vector<memory_range_t> makeRanges() const
    {
        std::vector<memory_range_t> resVec;
        std::map<address_t, bit_vector_t >::const_iterator it = m_fillMap.begin();
        for(; it!=m_fillMap.end(); ++it)
        {
            it->second.makeRanges(resVec, it->first);
        }

        return resVec;
    }

    static
    std::vector<memory_range_t> mergeRanges(std::vector<memory_range_t> &&rv1, std::vector<memory_range_t> &&rv2)
    {
        //return bit_vector_t::mergeRanges(std::forward(rv1), std::forward(rv2));
        return bit_vector_t::mergeRanges(std::move(rv1), std::move(rv2));
    }

    template<typename StreamType>
    static
    StreamType& printRanges(StreamType &oss, const std::vector<memory_range_t> &ranges, std::string offset=std::string())
    {
        bit_vector_t::printRanges(oss, ranges, offset);
        return oss;
    }


}; // class MemoryFillMap


template<typename StreamType>
StreamType& operator<<(StreamType &oss, const MemoryFillMap &mfm)
{
    return mfm.printTo(oss, MemoryFillMap::ostreamWideOutput);
}


// OutputIterator byteToHex(std::uint8_t b, OutputIterator oit, bool bLower=false)
// OutputIterator address32ToHex(std::uint32_t a, OutputIterator oit, bool bLower=false)



//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/types.h

