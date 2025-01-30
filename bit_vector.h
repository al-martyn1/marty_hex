/*! \file
    \brief BitVector class
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
template<typename BitIndexType>
class BitVector
{

public:

    using bit_index_t         = BitIndexType;
    using bit_index_range_t   = std::pair<bit_index_t, bit_index_t>;
    


protected:

    using bit_chunk_t = std::uint64_t;
    
    static
    constexpr const inline bit_index_t invalid_bit_index = bit_index_t(-1);

    std::vector<bit_chunk_t>    m_bits;
    std::size_t                 m_size = 0; // размер в битах

    static
    std::size_t calcChunkIndex(bit_index_t bitIndex)
    {
        // 2**4=16
        // 2**5=32
        // 2**6=64

        return bitIndex>>6;
    }

    static
    bit_chunk_t makeBitMask(bit_index_t bitIndex)
    {
        return bit_chunk_t(1) << (bitIndex&0x3F);
    }

    template<typename OutputIteratorType>
    static
    OutputIteratorType makeChunkRanges(bit_index_t chunkBaseIndex, bit_chunk_t chunk, OutputIteratorType oit)
    {
        bit_chunk_t mask = 1;

        //std::pair<bit_index_t, bit_index_t> curRange;
        //bool crValid = false;

        bit_index_t beginIdx = invalid_bit_index;
        bit_index_t endIdx   = invalid_bit_index;

        for(bit_index_t idx=0u; mask!=0; mask<<=1, ++idx)
        {
            if (chunk&mask)
            {
                // Бит установлен

                if (beginIdx==invalid_bit_index)
                {
                    beginIdx = chunkBaseIndex + idx; // Устанавливаем начало диапазона, раз было не задано
                }

                endIdx = beginIdx+1; // Обновляем конец диапазона
            }
            else // Бит сброшен
            {
                if (beginIdx!=invalid_bit_index) // Диапазон был задан
                {
                    *oit++ = std::make_pair(beginIdx, endIdx);
                    beginIdx = invalid_bit_index;
                    endIdx   = invalid_bit_index;
                }
            }
        }

        if (beginIdx!=invalid_bit_index) // Диапазон был задан
        {
            *oit++ = std::make_pair(beginIdx, endIdx);
        }

        return oit;
    }

    template<typename OutputIteratorType>
    OutputIteratorType makeRanges(bit_index_t baseIndex, OutputIteratorType oit) const
    {
        std::size_t idx = 0;

        std::size_t beginIdx = std::size_t(-1);
        std::size_t endIdx   = std::size_t(-1);

        for(; idx!=m_bits.size(); ++idx)
        {
            if (m_bits[idx]==bit_chunk_t(-1)) // Все биты установлены?
            {
                if (beginIdx==std::size_t(-1))
                {
                    beginIdx = idx;
                }
                endIdx = beginIdx+1;
            }
            else
            {
                if (beginIdx!=std::size_t(-1))
                {
                    *oit++ = std::make_pair(bit_index_t(baseIndex+bit_index_t(beginIdx<<6)), bit_index_t(baseIndex+bit_index_t(endIdx  <<6)));
                    beginIdx = std::size_t(-1);
                    endIdx   = std::size_t(-1);
                }

                if (m_bits[idx]!=0)
                {
                    oit = makeChunkRanges(baseIndex+bit_index_t(idx<<6), m_bits[idx], oit);
                }
            }
        }

        if (beginIdx!=std::size_t(-1))
        {
            *oit++ = std::make_pair(bit_index_t(baseIndex+bit_index_t(beginIdx<<6)), bit_index_t(baseIndex+bit_index_t(endIdx  <<6)));
        }

        return oit;
    }

    struct BitIndexRangesBackInsertIterator
    {
        std::vector<bit_index_range_t> &vec;

        MARTY_HEX_OUTPUT_ITERATOR_BILLET(BitIndexRangesBackInsertIterator);

        explicit BitIndexRangesBackInsertIterator(std::vector<bit_index_range_t> &v) : vec(v) {}
    
        BitIndexRangesBackInsertIterator& operator=(bit_index_range_t r)
        {
            if (vec.empty())
            {
                vec.emplace_back(r);
            }
            else
            {
                if (vec.back().second==r.first) // Конец предыдущего равен началу добавляемого
                    vec.back().second = r.second; // Сливаем вместе два диапазона
                else
                    vec.emplace_back(r);
            }

            return *this;
        }

    }; // struct BitIndexRangesBackInsertIterator


public:

    BitVector() = default;
    BitVector(const BitVector &) = default;
    BitVector(BitVector &&) = default;
    BitVector& operator=(const BitVector &) = default;
    BitVector& operator=(BitVector &&) = default;


    bit_index_t size() const { return bit_index_t(m_size); }
    bool empty() const { return m_size==0; }

    bool getBit(bit_index_t bitIndex) const
    {
        std::size_t chunkIdx = calcChunkIndex(bitIndex);
        if (chunkIdx>=m_bits.size())
            return false;
        
        return (m_bits[chunkIdx]&makeBitMask(bitIndex)) != 0u;
    }

    void setBit(bit_index_t bitIndex, bool bVal)
    {
        std::size_t chunkIdx = calcChunkIndex(bitIndex);
        if (chunkIdx>=m_bits.size())
        {
            m_bits.resize(chunkIdx+1u, 0u);
        }

        if (m_size<std::size_t(bitIndex+1))
            m_size = std::size_t(bitIndex)+1u;

        bit_chunk_t mask = makeBitMask(bitIndex);

        if (bVal)
            m_bits[chunkIdx] |=  mask;
        else
            m_bits[chunkIdx] &= ~mask;
    }


    void makeRanges(std::vector<bit_index_range_t> &resVec, bit_index_t baseIndex) const
    {
        makeRanges(baseIndex, BitIndexRangesBackInsertIterator(resVec));
    }

    static
    std::vector<bit_index_range_t> mergeRanges(std::vector<bit_index_range_t> &&rv1, std::vector<bit_index_range_t> &&rv2)
    {
        if (rv1.empty())
            return rv2;
        if (rv2.empty())
            return rv1;
        if (rv1.back().second==rv2.front().first)
        {
            rv2.front().first = rv1.back().first;
            rv1.pop_back();
        }

        rv1.insert(rv1.end(), rv2.begin(), rv2.end());

        return rv1;
    }

    template<typename StreamType>
    static
    StreamType& printRanges(StreamType &oss, const std::vector<bit_index_range_t> &ranges)
    {
        for(auto &&r:ranges)
        {
            std::string strAddr1;
            utils::address32ToHex(r.first   , std::back_inserter(strAddr1));
            std::string strAddr2;
            utils::address32ToHex(r.second-1, std::back_inserter(strAddr2));
            oss << strAddr1 << "-" << strAddr2 << "\n";
        }
    }


    // Надо уметь печатать битики. По 64 в строку, в начале идёт адрес базы, потом двоеточие

    // template<typename OutputIteratorType>
    // OutputIteratorType makeRanges(bit_index_t baseIndex, OutputIteratorType oit)



}; // class BitVector

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/types.h

