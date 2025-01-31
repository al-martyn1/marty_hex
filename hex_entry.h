/*! \file
    \brief Hex file data parsing (Intel HEX)
 */

#pragma once

//----------------------------------------------------------------------------
#include "enums.h"
#include "file_pos_info.h"
#include "hex_info.h"
#include "utils.h"
#include "types.h"
#include "memory_fill_map.h"

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
/*
    https://ru.wikipedia.org/wiki/Intel_HEX
    https://en.wikipedia.org/wiki/Intel_HEX

    Файл состоит из текстовых ASCII-строк. Каждая строка представляет собой одну запись. Каждая запись начинается с двоеточия (:),
    после которого идёт набор шестнадцатеричных цифр кратных байту:

    Начало записи (:).
    Количество байт данных, содержащихся в этой записи. Занимает один байт (две шестнадцатеричных цифры), что соответствует 0…255 в десятичной системе.
    Начальный адрес блока записываемых данных — 2 байта. Этот адрес определяет абсолютное местоположение данных этой записи в двоичном файле.
    Один байт, обозначающий тип записи. Определены следующие типы записей:
        0 — запись содержит данные двоичного файла.
        1 — запись обозначает конец файла, данных не содержит. Имеет характерный вид «:00000001FF».
        2 — запись адреса сегмента (подробнее см.ниже).
        4 — запись расширенного адреса (подробнее см.ниже).
    Байты данных, которые требуется сохранить в EPROM (их число указывается в начале записи, от 0 до 255 байт).
    Последний байт в записи является контрольной суммой. Рассчитывается так чтобы сумма всех байтов в записи была равна 0.
    Строка заканчивается стандартной парой CR/LF (0Dh 0Ah).

:105B20000102030406070809020406080000000039
:04000005080031516D
:00000001FF

:LLAAAATTDD....CC

    Каждая группа букв (LL, AAAA, TT и т. д.) представляет собой отдельное поле. Каждая буква — отдельную 16-ричную цифру (4 бита). 
    Каждое поле состоит как минимум из двух 16-ричных цифр (один байт). Ниже представлена расшифровка полей записи:

    : Каждая запись в файле Intel HEX должна начинаться с двоеточия.
    LL Поле длины — показывает количество байт данных (DD) в записи.
    AAAA Поле адреса — представляет начальный адрес записи.
    TT Поле типа. Оно может принимать следующие значения:
        00 запись содержит данные двоичного файла.
        01 запись является концом файла.
        02 запись адреса сегмента (подробнее см. ниже).
        03 Стартовый адрес запуска программы в формате 80x86 (значения регистров CS:IP счетчика команд и регистра смещения)
        04 запись расширенного адреса (подробнее см. ниже).
        05 32-битный адрес запуска программы в «линейном» адресном пространстве
    DD Поле данных. Запись может содержать несколько байт данных. Количество байт данных должно соответствовать полю LL.
    CC Поле контрольной суммы. Поле контрольной суммы вычисляется путём сложения значений всех байт (пар 16-ричных цифр) 
    записи по модулю 256 с последующим переводом в дополнительный формат (отнять получившееся значение от 0). Таким образом, 
    если просуммировать все пары шестнадцатеричных чисел, включая LL, AA, TT, DD, CC, получится 0.

*/

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! При разборе сначала все байты кладутся в массив data, и только по окончании строки производится разбор на составляющие (перед этим проверяется КС), и лишнее удаляется
struct HexEntry
{
    std::uint8_t      numDataBytes = 0;
    std::uint16_t     address      = 0;
    HexRecordType     recordType   = HexRecordType::invalid;
    byte_vector       data;
    std::uint8_t      checksum     = 0;

    // std::size_t       lineNo = 0;
    // std::size_t       fileId = std::size_t(-1);
    FilePosInfo       filePosInfo;
    // std::uint32_t     effectiveAddress = 0;
    std::uint16_t     baseAddress = 0;
    AddressMode       addressMode = AddressMode::none;

    
    HexEntry() = default;
    HexEntry(const HexEntry&) = default;
    HexEntry(HexEntry&&) = default;
    HexEntry& operator=(const HexEntry&) = default;
    HexEntry& operator=(HexEntry&&) = default;

    explicit HexEntry(HexRecordType rt) : recordType(rt)
    {
        if ( rt!=HexRecordType::invalid
          && rt!=HexRecordType::eof
           )
           throw std::runtime_error("HexEntry::HexEntry(HexRecordType rt): rt can be only 'invalid' or 'eof'");
    }

    explicit HexEntry(HexRecordType rt, const byte_vector &bv) : numDataBytes(std::uint8_t(bv.size())), recordType(rt), data(bv)
    {
        if ( rt!=HexRecordType::data)
           throw std::runtime_error("HexEntry::HexEntry(HexRecordType rt, const byte_vector &bv): rt can be only 'data'");
        if (bv.size()>255)
            throw std::runtime_error("HexEntry::HexEntry(HexRecordType rt, const byte_vector &bv): 'bv' too big");
    }

    explicit HexEntry(const byte_vector &bv) : numDataBytes(std::uint8_t(bv.size())), recordType(HexRecordType::data), data(bv)
    {
        if (bv.size() > 255)
            throw std::runtime_error("HexEntry::HexEntry(HexRecordType rt, const byte_vector &bv): 'bv' too big");
    }

    explicit HexEntry(HexRecordType rt, const std::uint16_t addrHi) : numDataBytes(2), recordType(rt)
    {
        if ( rt!=HexRecordType::extendedSegmentAddress
          && rt!=HexRecordType::extendedLinearAddress
           )
           throw std::runtime_error("HexEntry::HexEntry(HexRecordType rt, const std::uint16_t addrHi): rt can be only 'extendedSegmentAddress' or 'extendedLinearAddress'");

        appendDataByte(std::uint8_t(addrHi>>8));
        appendDataByte(std::uint8_t(addrHi));

        // if (rt!=HexRecordType::extendedSegmentAddress)
        //     addressMode = AddressMode::sba;
        // else if (rt!=HexRecordType::extendedLinearAddress)
        //     addressMode = AddressMode::lbs;
    }

    explicit HexEntry(HexRecordType rt, const std::uint32_t addrHi) : numDataBytes(4), recordType(rt)
    {
        if ( rt!=HexRecordType::startSegmentAddress
          && rt!=HexRecordType::extendedLinearAddress
           )
           throw std::runtime_error("HexEntry::HexEntry(HexRecordType rt, const std::uint32_t addrHi): rt can be only 'startSegmentAddress' or 'startLinearAddress'");

        appendDataByte(std::uint8_t(addrHi>>24));
        appendDataByte(std::uint8_t(addrHi>>16));
        appendDataByte(std::uint8_t(addrHi>>8));
        appendDataByte(std::uint8_t(addrHi));
    }


    // Мы игнорируем тип записи при очистке и при проверке на пустоту, для того, чтобы у нас оставался тип последней прочитанной записи

    void reset()
    {
        numDataBytes = 0;
        address      = 0;
        //recordType   = HexRecordType::invalid;
        data.clear();
        checksum     = 0;
    }

    void clear() { reset(); }

    bool empty() const
    {
        return numDataBytes==0 && address==0  /* && recordType==HexRecordType::eof */  && data.empty() && checksum==0;
    }

    void appendDataByte(std::uint8_t b)
    {
        intVectorAppendHelper(data, b);
    }

    HexEntry makeFitCopy() const
    {
        HexEntry res = *this;
        //res.data.shrink_to_fit();
        byte_vector dataTmp = byte_vector(res.data.begin(), res.data.end());
        res.data.swap(dataTmp);
        return res;
    }

    static
    std::uint8_t calcChecksum(const std::uint8_t *pData, std::size_t size)
    {
        std::uint8_t csum = 0;
        for(auto idx=0u; idx!=size; ++idx)
        {
            std::uint8_t b = pData[idx];
            csum += b;
        }
    
        return (std::uint8_t)(0u - (unsigned)csum);
    }

    bool isEof() const
    {
        return recordType==HexRecordType::eof;
    }

    bool hasAddress() const
    {
        return (recordType==HexRecordType::data || recordType==HexRecordType::eof || recordType==HexRecordType::invalid) ? false : true;
    }

    std::string getFullAddressString() const
    {
        // std::string res;
        // std::uint8_t cs = 0;
        // (void)cs;
        // auto oit = utils::HexOutputIterator(res, cs);
        // *oit++ = std::uint8_t(effectiveAddress>>24);
        // *oit++ = std::uint8_t(effectiveAddress>>16);
        // *oit++ = std::uint8_t(effectiveAddress>>8);
        // *oit++ = std::uint8_t(effectiveAddress);
        // return res;

        return utils::address32ToString((std::uint32_t(baseAddress)<<16)+address, addressMode);
    }

    std::string getDataDumpString() const
    {
        std::string res; res.reserve(data.size()*3u);
        std::uint8_t cs = 0;
        auto oit = utils::HexOutputIterator(res, cs);

        for(auto &&b : data)
        {
            if (!res.empty())
                res.append(1, ' ');
            *oit++ = b;
        }

        return res;
    }

    std::string getTypeAbbrString() const
    {
        switch(recordType)
        {
            case HexRecordType::invalid               : return "INV"; 
            case HexRecordType::data                  : return "DATA";
            case HexRecordType::eof                   : return "EOF"; 
            case HexRecordType::extendedSegmentAddress: return "ESA"; 
            case HexRecordType::startSegmentAddress   : return "SSA"; 
            case HexRecordType::extendedLinearAddress : return "ELA"; 
            case HexRecordType::startLinearAddress    : return "SLA"; 
        }
        return std::string();
    }

    std::string toString() const // make human readable string
    {
        return getFullAddressString() + ": " + getDataDumpString() + " ; " + getTypeAbbrString();
    }


protected:

    std::uint32_t calcDataByteAddress(std::size_t byteIndex, std::uint16_t a_baseAddr , AddressMode a_addressMode) const
    {
        if (recordType!=HexRecordType::data)
            throw std::runtime_error("HexEntry::calcDataAddress - not a data record");

        if (byteIndex>=data.size())
            throw std::runtime_error("HexEntry::calcDataAddress - byte index is out of range");

        // auto baseAddr = effectiveAddress;

        switch(a_addressMode)
        {
            case AddressMode::none:
                 return (std::uint32_t(a_baseAddr)<<16) + address + std::uint32_t(byteIndex);

            case AddressMode::sba:
                 return (std::uint32_t(a_baseAddr)<<4 ) + std::uint32_t(std::uint16_t(std::uint32_t(address) + std::uint32_t(byteIndex)));

            case AddressMode::lba:
                 return (std::uint32_t(a_baseAddr)<<16) + address + std::uint32_t(byteIndex);

        }

        throw std::runtime_error("HexEntry::calcDataAddress - unknown addressMode");

        // return 0;
    }


public:

    std::uint32_t getDataByteAddress(std::size_t byteIndex) const
    {
        return calcDataByteAddress(byteIndex, baseAddress , addressMode);
    }

    std::uint32_t getEffectiveBaseAddress() const
    {
        switch(addressMode)
        {
            case AddressMode::none:
                 return (std::uint32_t(baseAddress)<<16) + address;

            case AddressMode::sba:
                 return (std::uint32_t(baseAddress)<<4 ) + std::uint32_t(address);

            case AddressMode::lba:
                 return (std::uint32_t(baseAddress)<<16) + address;

        }

        throw std::runtime_error("HexEntry::getEffectiveBaseAddress - unknown addressMode");
    }



    //std::uint16_t     baseAddress = 0;
    //AddressMode       addressMode = AddressMode::none;

    //std::uint32_t calcDataByteAddress(std::size_t byteIndex, std::uint32_t a_baseAddr , AddressMode a_addressMode) const

    

    std::string serialize(bool dontPrependColon=false)
    {
        //if (recordType==HexRecordType::invalid)

        std::string res; res.reserve(1u+2u*(5u+data.size())); // +colon
        if (!dontPrependColon)
            res = ":";

        std::uint8_t cs = 0;
        auto oit = utils::HexOutputIterator(res, cs);

        switch(recordType)
        {
            case HexRecordType::invalid: return std::string();

            case HexRecordType::data:
                 *oit++ = std::uint8_t(data.size());
                 *oit++ = std::uint8_t(address>>8);
                 *oit++ = std::uint8_t(address   );
                 break;

            case HexRecordType::eof:
                 *oit++ = std::uint8_t(0); *oit++ = std::uint8_t(0); *oit++ = std::uint8_t(0);
                 break;

            case HexRecordType::extendedSegmentAddress:
                 *oit++ = std::uint8_t(2u); *oit++ = std::uint8_t(0); *oit++ = std::uint8_t(0);
                 break;

            case HexRecordType::startSegmentAddress:
                 *oit++ = std::uint8_t(4u); *oit++ = std::uint8_t(0); *oit++ = std::uint8_t(0);
                 break;

            case HexRecordType::extendedLinearAddress:
                 *oit++ = std::uint8_t(2u); *oit++ = std::uint8_t(0); *oit++ = std::uint8_t(0);
                 break;

            case HexRecordType::startLinearAddress:
                 *oit++ = std::uint8_t(4u); *oit++ = std::uint8_t(0); *oit++ = std::uint8_t(0);
                 break;
        }

        *oit++ = std::uint8_t(recordType);

        for(auto &&b : data)
            *oit++ = b;

        cs = (std::uint8_t)(0u - (unsigned)cs);
        *oit++ = cs;

        return res;
    }

    // 

    // Только для записей, содержащих адрес
    std::uint16_t extractBaseAddressFromDataBytes() const
    {
        // Вообще мы размер данных проверяли при разборе, но, чтобы не упасть, тут на всякий случай вернём 0, если что-то не так.
        // TODO: наверное надо бы ассерт вставить
        switch(recordType)
        {
            case HexRecordType::invalid: return 0;

            case HexRecordType::data: return 0;

            case HexRecordType::eof:  return 0;

            case HexRecordType::extendedSegmentAddress:
                 if (data.size()!=2)
                     return 0;
                 return std::uint16_t(((std::uint16_t(data[0])<<8) + std::uint16_t(data[1])));

            case HexRecordType::startSegmentAddress: return 0;

            case HexRecordType::extendedLinearAddress:
                 if (data.size()!=2)
                     return 0;
                 return std::uint16_t(((std::uint16_t(data[0])<<8) + std::uint16_t(data[1])));

            case HexRecordType::startLinearAddress: return 0;
        }

        return 0;
    }

    // Только для записей, содержащих адрес
    std::uint32_t extractStartAddressFromDataBytes() const
    {
        // Вообще мы размер данных проверяли при разборе, но, чтобы не упасть, тут на всякий случай вернём 0, если что-то не так.
        // TODO: наверное надо бы ассерт вставить
        switch(recordType)
        {
            case HexRecordType::invalid: return 0;

            case HexRecordType::data: return 0;

            case HexRecordType::eof:  return 0;

            case HexRecordType::extendedSegmentAddress: return 0;

            case HexRecordType::startSegmentAddress: // сегмент - в старших байтах
                 if (data.size()!=4)
                     return 0;
                 return (std::uint32_t(data[0])<<24) + (std::uint32_t(data[1])<<16)
                      + (std::uint32_t(data[2])<<8 ) + (std::uint32_t(data[3]));

            case HexRecordType::extendedLinearAddress: return 0;

            case HexRecordType::startLinearAddress:
                 if (data.size()!=4)
                     return 0;
                 return (std::uint32_t(data[0])<<24) + (std::uint32_t(data[1])<<16)
                      + (std::uint32_t(data[2])<<8 ) + (std::uint32_t(data[3]));
        }

        return 0;
    }



    bool parseRawData(ParsingResult &r, HexInfo *pHexInfo)
    {
        if (data.size()<5)
            return r=ParsingResult::tooFewBytes, false;

        std::uint8_t csumCalculated = calcChecksum(data.data(), data.size()-1);
        std::uint8_t csumReaded     = data.back();
        if (csumCalculated!=csumReaded)
            return r=ParsingResult::checksumMismatch, false;

        checksum = csumCalculated;
        intVectorEraseHelper( data, data.size()-1,1);

        numDataBytes = data[0];

        address = (std::uint16_t)data[1];
        address <<= 8;
        address |= (std::uint16_t)data[2];

        recordType = (HexRecordType)data[3];

        intVectorEraseHelper( data, 0, 4);

        if (data.size()>(std::size_t)numDataBytes)
            return r=ParsingResult::tooManyDataBytes, false;

        if (data.size()<(std::size_t)numDataBytes)
            return r=ParsingResult::tooFewDataBytes, false;

        // Проверяем количество байт данных типу записи
        switch(recordType)
        {
            case HexRecordType::invalid: break;

            case HexRecordType::data: break; // нет констрайнов

            case HexRecordType::eof:
                 if (numDataBytes!=0)
                     return r=ParsingResult::dataSizeNotMatchRecordType, false;
                 break;

            case HexRecordType::extendedSegmentAddress:
                 if (numDataBytes!=2)
                     return r=ParsingResult::dataSizeNotMatchRecordType, false;

                 if (pHexInfo && pHexInfo->addressMode==AddressMode::none)
                 {
                     pHexInfo->addressMode = AddressMode::sba;
                     pHexInfo->baseAddress = std::uint32_t(extractBaseAddressFromDataBytes())<<16;
                 }

                 break;

            case HexRecordType::startSegmentAddress:
                 if (numDataBytes!=4)
                     return r=ParsingResult::dataSizeNotMatchRecordType, false;
                 if (pHexInfo && pHexInfo->startAddress==std::uint32_t(-1))
                 {
                     pHexInfo->startAddress = extractStartAddressFromDataBytes();
                 }

                 break;

            case HexRecordType::extendedLinearAddress:
                 if (numDataBytes!=2)
                     return r=ParsingResult::dataSizeNotMatchRecordType, false;
                 if (pHexInfo && pHexInfo->addressMode==AddressMode::none)
                 {
                     pHexInfo->addressMode = AddressMode::lba;
                     pHexInfo->baseAddress = std::uint32_t(extractBaseAddressFromDataBytes())<<16;
                 }
                 break;

            case HexRecordType::startLinearAddress:
                 if (numDataBytes!=4)
                     return r=ParsingResult::dataSizeNotMatchRecordType, false;
                 if (pHexInfo && pHexInfo->startAddress==std::uint32_t(-1))
                 {
                     pHexInfo->startAddress = extractStartAddressFromDataBytes();
                 }
                 break;

            default:
                 return r=ParsingResult::unknownRecordType, false;
                 
        }

        return true; // ParsingResult::ok;
        
    }

    // TODO: Надо завести поле под линейный адрес и вычислить его для всех записей в результирующем массиве


}; // struct HexEntry

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/marty_hex.h



