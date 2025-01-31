/*! \file
    \brief Hex file data parsing (Intel HEX)
 */

#pragma once

//----------------------------------------------------------------------------
#include "enums.h"
#include "file_pos_info.h"
#include "hex_entry.h"
#include "intel_hex_parser.h"
#include "memory_fill_map.h"
#include "types.h"
#include "utils.h"

//----------------------------------------------------------------------------
#include <string>
#include <cstdint>
#include <vector>
#include <exception>
#include <stdexcept>

//----------------------------------------------------------------------------


// marty_hex/marty_hex.h
// marty::hex::
namespace marty{
namespace hex{

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

// Заодно обновляем поле адрес address значением ULBA/USBA. А надо ли? Наверное, не надо
inline
void updateHexEntriesAddressAndMode(std::vector<HexEntry> &heVec)
{
    std::uint16_t curBaseAddr = 0;
    std::uint32_t nextAddr    = 0;
    AddressMode   addressMode = AddressMode::none;

    for(auto &he : heVec)
    {
        he.addressMode = addressMode;
        he.baseAddress = curBaseAddr;
        if (he.recordType!=HexRecordType::data)
            he.address = std::uint16_t(nextAddr);

        switch(he.recordType)
        {
            case HexRecordType::invalid:
                 break;

            case HexRecordType::data:
                 nextAddr = he.address + std::uint32_t(he.data.size());
                 break;

            case HexRecordType::eof:
                 break;

            case HexRecordType::extendedSegmentAddress:
                 addressMode    = AddressMode::sba;
                 he.addressMode = addressMode;
                 curBaseAddr    = he.extractBaseAddressFromDataBytes();
                 he.baseAddress = curBaseAddr;
                 break;

            case HexRecordType::startSegmentAddress:
                 break;

            case HexRecordType::extendedLinearAddress:
                 addressMode    = AddressMode::lba;
                 he.addressMode = addressMode;
                 curBaseAddr    = he.extractBaseAddressFromDataBytes();
                 he.baseAddress = curBaseAddr;
                 break;

            case HexRecordType::startLinearAddress:
                 break;
        }

    }
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct HexRecordsCheckResultEntry
{
    HexRecordsCheckCode   code;
    FilePosInfo           filePosInfo;

}; // struct HexFileCheckResultEntry

using HexRecordsCheckReport = std::vector<HexRecordsCheckResultEntry>;


inline
HexRecordsCheckCode checkHexRecords(const std::vector<HexEntry> &heVec, MemoryFillMap *pMemMap, HexRecordsCheckReport *pReport)
{
    MemoryFillMap memoryFillMap;

    //std::uint32_t curBaseAddr = 0;
    //std::uint32_t nextAddr    = 0; (void)nextAddr;
    AddressMode   addressMode = AddressMode::none;

    HexRecordsCheckReport report;
    bool overlapsReported = false;
    HexRecordsCheckCode resCode = HexRecordsCheckCode::none;

    for(auto &&he : heVec)
    {
        switch(he.recordType)
        {
            case HexRecordType::invalid: break;

            case HexRecordType::data:
                 //nextAddr = he.effectiveAddress + std::uint32_t(he.data.size());

                 //std::size_t dataSize = he.data.size();
                 //for(std::size_t i=0; i!=dataSize; ++i)
                 for(std::size_t i=0; i!=he.data.size(); ++i)
                 {
                     auto addr = he.getDataByteAddress(i); // he.calcDataByteAddress(i, curBaseAddr, addressMode);
                     if (memoryFillMap.getFilled(addr))
                     {
                         if (!overlapsReported)
                         {
                             overlapsReported = true;
                             report.emplace_back(HexRecordsCheckResultEntry{HexRecordsCheckCode::memoryOverlaps, he.filePosInfo});
                             resCode |= HexRecordsCheckCode::memoryOverlaps;
                         }
                     }

                     memoryFillMap.setFilled(addr);
                 }
                 break;

            case HexRecordType::eof: break;

            case HexRecordType::extendedSegmentAddress:
                 if (addressMode!=AddressMode::none && addressMode!=AddressMode::sba)
                 {
                     report.emplace_back(HexRecordsCheckResultEntry{HexRecordsCheckCode::mixedAddressMode, he.filePosInfo});
                     resCode |= HexRecordsCheckCode::mixedAddressMode;
                 }
                 addressMode = AddressMode::sba;
                 //curBaseAddr = he.extractAddressFromDataBytes();
                 //nextAddr    = curBaseAddr;
                 break;

            case HexRecordType::startSegmentAddress:
                 if (addressMode!=AddressMode::none && addressMode!=AddressMode::sba)
                 {
                     report.emplace_back(HexRecordsCheckResultEntry{HexRecordsCheckCode::mixedAddressMode, he.filePosInfo});
                     resCode |= HexRecordsCheckCode::mixedAddressMode;
                 }
                 break;

            case HexRecordType::extendedLinearAddress:
                 if (addressMode!=AddressMode::none && addressMode!=AddressMode::lba)
                 {
                     report.emplace_back(HexRecordsCheckResultEntry{HexRecordsCheckCode::mixedAddressMode, he.filePosInfo});
                     resCode |= HexRecordsCheckCode::mixedAddressMode;
                 }
                 addressMode = AddressMode::lba;
                 //curBaseAddr = he.extractAddressFromDataBytes();
                 //nextAddr    = curBaseAddr;
                 break;

            case HexRecordType::startLinearAddress:
                 if (addressMode!=AddressMode::none && addressMode!=AddressMode::lba)
                 {
                     report.emplace_back(HexRecordsCheckResultEntry{HexRecordsCheckCode::mixedAddressMode, he.filePosInfo});
                     resCode |= HexRecordsCheckCode::mixedAddressMode;
                 }
                 break;
        }

    }

    if (pMemMap)
       *pMemMap = memoryFillMap;

    if (!report.empty())
    {
        if (pReport)
           *pReport = report;
    }

    return resCode;
}

inline
void normalizeAddressOrder(std::vector<HexEntry> &heVec)
{
    std::stable_sort( heVec.begin(), heVec.end()
                    , [](const HexEntry &e1, const HexEntry &e2)
                      {
                          return e1.getEffectiveBaseAddress() < e2.getEffectiveBaseAddress();
                      }
                    );
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/marty_hex.h



