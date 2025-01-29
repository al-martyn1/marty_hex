/*! \file
    \brief Hex file data parsing (Intel HEX)
 */

#pragma once

//----------------------------------------------------------------------------
#include "enums.h"
#include "utils.h"
#include "types.h"

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


    std::size_t       lineNo = 0;
    std::uint32_t     effectiveAddress = 0;

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
        return (recordType==HexRecordType::data || recordType==HexRecordType::eof) ? false : true;
    }

    std::string getEffectiveAddressString() const
    {
        std::string res;
        std::uint8_t cs = 0;
        (void)cs;
        auto oit = utils::HexOutputIterator(res, cs);
        *oit++ = std::uint8_t(effectiveAddress>>24);
        *oit++ = std::uint8_t(effectiveAddress>>16);
        *oit++ = std::uint8_t(effectiveAddress>>8);
        *oit++ = std::uint8_t(effectiveAddress);
        return res;
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
        return getEffectiveAddressString() + ": " + getDataDumpString() + " ; " + getTypeAbbrString();
    }

    std::uint32_t calcDataAddress(std::size_t byteIndex, std::uint32_t baseAddr, AddressMode addressMode)
    {
        if (recordType!=HexRecordType::data)
            throw std::runtime_error("HexEntry::calcDataAddress - not a data record");

        if (byteIndex>=data.size())
            throw std::runtime_error("HexEntry::calcDataAddress - byte index is out of range");

        // TODO: надо дописать

        switch(addressMode)
        {
            case AddressMode::sba:
            case AddressMode::lba:
        }
    }

                 // if (addressMode==AddressMode::sba)
                 // {
                 //     he.effectiveAddress = curBaseAddr + he.address; // Для стартового байта ничего не меняется, а вот для байт, которые после него - меняется, они могут завернуться на начало сегмента
                 // }
                 // else
                 // {
                 //     he.effectiveAddress = curBaseAddr + he.address; // ByteAddr = (LBA + DRLO + DRI) mod 4G, https://spd.net.ru/Article/Intel-HEX
                 // }

    // std::uint32_t curBaseAddr = 0;
    // std::uint32_t nextAddr    = 0;
    // AddressMode addressMode   = AddressMode::sba;
    

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
    std::uint32_t extractAddressFromDataBytes() const
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
                 return ((std::uint32_t(data[0])<<8) + std::uint32_t(data[1])) << 4;

            case HexRecordType::startSegmentAddress:
                 if (data.size()!=4)
                     return 0;
                 return ( ( (std::uint32_t(data[0])<<8) + (std::uint32_t(data[1])) ) << 4 ) |
                        ( (std::uint32_t(data[2])<<8) + (std::uint32_t(data[3])) );

            case HexRecordType::extendedLinearAddress:
                 if (data.size()!=2)
                     return 0;
                 return ((std::uint32_t(data[0])<<8) + std::uint32_t(data[1])) << 16;

            case HexRecordType::startLinearAddress:
                 if (data.size()!=4)
                     return 0;
                 return (std::uint32_t(data[0])<<24) + (std::uint32_t(data[1])<<16)
                      + (std::uint32_t(data[2])<<8 ) + (std::uint32_t(data[3]));
        }

        return 0;
    }


    bool parseRawData(ParsingResult &r)
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
                 break;


            case HexRecordType::startSegmentAddress:
                 if (numDataBytes!=4)
                     return r=ParsingResult::dataSizeNotMatchRecordType, false;
                 break;

            case HexRecordType::extendedLinearAddress:
                 if (numDataBytes!=2)
                     return r=ParsingResult::dataSizeNotMatchRecordType, false;
                 break;

            case HexRecordType::startLinearAddress:
                 if (numDataBytes!=4)
                     return r=ParsingResult::dataSizeNotMatchRecordType, false;
                 break;
        }

        return true; // ParsingResult::ok;
        
    }

    // TODO: Надо завести поле под линейный адрес и вычислить его для всех записей в результирующем массиве


}; // struct HexEntry

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Заодно обновляем поле адрес address значением ULBA/USBA. А надо ли? Наверное, не надо
inline
void updateHexEntriesEffectiveAddress(std::vector<HexEntry> &heVec)
{
    std::uint32_t curBaseAddr = 0;
    std::uint32_t nextAddr    = 0;
    AddressMode addressMode   = AddressMode::none;

    // std::uint16_t     address      = 0;
    // HexRecordType     recordType   = HexRecordType::invalid;
    // byte_vector       data;


    for(auto &he : heVec)
    {
        switch(he.recordType)
        {
            case HexRecordType::invalid:
                 he.effectiveAddress = nextAddr;
                 break;

            case HexRecordType::data:
                 if (addressMode==AddressMode::sba)
                 {
                     he.effectiveAddress = curBaseAddr + he.address; // Для стартового байта ничего не меняется, а вот для байт, которые после него - меняется, они могут завернуться на начало сегмента
                 }
                 else if (addressMode==AddressMode::lba)
                 {
                     he.effectiveAddress = curBaseAddr + he.address; // ByteAddr = (LBA + DRLO + DRI) mod 4G, https://spd.net.ru/Article/Intel-HEX
                 }
                 else
                 {
                     throw std::runtime_error("undefined address mode");
                 }

                 nextAddr = he.effectiveAddress + std::uint32_t(he.data.size());
                 break;

            case HexRecordType::eof:
                 he.effectiveAddress = nextAddr;
                 break;

            case HexRecordType::extendedSegmentAddress:
                 addressMode = AddressMode::sba;
                 curBaseAddr = he.extractAddressFromDataBytes();
                 nextAddr    = curBaseAddr;
                 he.effectiveAddress = curBaseAddr;
                 break;

            case HexRecordType::startSegmentAddress:
                 he.effectiveAddress = nextAddr; // curBaseAddr;
                 break;

            case HexRecordType::extendedLinearAddress:
                 addressMode = AddressMode::lba;
                 curBaseAddr = he.extractAddressFromDataBytes();
                 nextAddr    = curBaseAddr;
                 he.effectiveAddress = curBaseAddr;
                 break;

            case HexRecordType::startLinearAddress:
                 he.effectiveAddress = nextAddr; // curBaseAddr;
                 break;
        }
    }
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
class HexParser
{

    enum State
    {
        waitStart         ,
        skipCommentLine   ,
        waitLf            ,
        waitFirstTetrad   ,
        waitSecondTetrad
    };

    State st = waitStart;
    HexEntry curEntry;


public:

    // Global within all chunks
    std::size_t line = 0;
    std::size_t pos  = 0;

    void reset()
    {
        curEntry.reset();
        line = 0;
        pos  = 0;
        st = waitStart;
    }

    void clear() { reset(); }


    bool moveIndexToNextLine(const std::string &hexText, std::size_t &idx) const
    {
        return moveIndexToNextLine(hexText.data(), hexText.size(), idx);

    }


    //! Если мы получили запись типа EOF, нам надо промотать концы строк, с подсчетом номера строки
    //! Если какая-то хрень, то возвращаем false, пусть вызывающий делает assert, если ему это надо
    //! Это всё нужно для корректного подсчета номера строки, если он не нужен, то можно забить 
    bool moveIndexToNextLine(const char* pData, std::size_t size, std::size_t &idx) const
    {
        // Мы находимся либо в состоянии waitStart - у нас был LF перевод строки, и ничего делать не нужно
        // можно сразу вычитывать новый HEX
        if (st==waitStart)
            return true;

        // либо мы в состоянии waitLf - у нас был CRLF перевод строки, мы по CR завершили чтение curEntry,
        // распарсили её и вернули ok, и остались в состоянии ожидания LF
        // Если состояние не waitLf - то это хрень
        if (st!=waitLf)
            return false;

        if (idx>=size)
            return true;

        if (pData[idx]=='\n')
            ++idx;

        return true;
    }

    ParsingResult parseFinalize(std::vector<HexEntry> &resVec)
    {
        switch(st)
        {
            case waitStart       :
                 return ParsingResult::unexpectedEnd;

            case skipCommentLine :
                 return ParsingResult::unexpectedEnd;

            case waitLf          :
                 return ParsingResult::unexpectedEnd;

            case waitFirstTetrad :
                 if (!curEntry.empty())
                 {
                     ParsingResult parseRes = ParsingResult::ok;
                     if (!curEntry.parseRawData(parseRes)) // Если что-то пошло не так, то мы получим false и в parseRes код возврата, его и возвращаем
                         return parseRes;
         
                     curEntry.lineNo = line;
                     resVec.emplace_back(curEntry.makeFitCopy());
                     curEntry.clear();
                 }
                 //return ParsingResult::notDigit;
                 return ParsingResult::unexpectedEnd;
            
            case waitSecondTetrad:
                 if (!curEntry.empty())
                 {
                     ParsingResult parseRes = ParsingResult::ok;
                     if (!curEntry.parseRawData(parseRes)) // Если что-то пошло не так, то мы получим false и в parseRes код возврата, его и возвращаем
                         return parseRes;
         
                     curEntry.lineNo = line;
                     resVec.emplace_back(curEntry.makeFitCopy());
                     curEntry.clear();
                 }
                 return ParsingResult::brokenByte;

            default:
                 return ParsingResult::invalidRecord;
        }
    }

    ParsingResult parseTextChunk( std::vector<HexEntry> &resVec
                                , const std::string &hexText
                                , std::size_t startIdx = 0
                                , ParsingOptions parsingOptions = ParsingOptions::none
                                , std::size_t *pErrorOffset=0
                                )
    {
        return parseTextChunk(resVec, hexText.data(), hexText.size(), startIdx, parsingOptions, pErrorOffset);
    }

    ParsingResult parseTextChunk( std::vector<HexEntry> &resVec
                                , const char* pData     // ptr to text chunk start
                                , std::size_t size      // text chunk start
                                , std::size_t startIdx = 0
                                , ParsingOptions parsingOptions = ParsingOptions::none
                                , std::size_t *pErrorOffset=0
                                )
    {
        std::size_t  idx     = startIdx;
        std::uint8_t curByte = 0;

        bool allowComments = (parsingOptions&ParsingOptions::allowComments)!=0;
        bool allowSpaces   = (parsingOptions&ParsingOptions::allowSpaces  )!=0;
        bool allowMultiHex = (parsingOptions&ParsingOptions::allowMultiHex)!=0;

        auto returnError = [&](ParsingResult e)
        {
            if (pErrorOffset)
                *pErrorOffset = idx;
            return e;
        };

        if (!pData || startIdx>size)
            returnError(ParsingResult::invalidArgument);



        for(; idx!=size; ++idx)
        {
            char ch = pData[idx];

            switch(st)
            {
                explicit_waitStart:
                case waitStart:
                {
                    if (ch==':')
                    {
                       ++pos;
                       st = waitFirstTetrad;
                       break;
                    }

                    else if (ch=='#')
                    {
                        if (allowComments)
                        {
                            st = skipCommentLine;
                            ++pos;
                            break;
                        }
                    }

                    else if (ch==' ')
                    {
                        if (allowSpaces) // keep st as waitStart
                        {
                            ++pos;
                            break;
                        }
                    }

                    else if (ch=='\r')
                    {
                        st = waitLf;
                        ++pos;
                        break;
                    }

                    else if (ch=='\n')
                    {
                        ++line;
                        pos = 0;
                        break;
                    }

                    return returnError(ParsingResult::invalidRecord); // Что-то непонятное пришло
                }
    
                case skipCommentLine:
                {
                    ++pos;

                    if (ch=='\r')
                    {
                        st = waitLf;
                        break;
                    }

                    else if (ch=='\n')
                    {
                        ++line;
                        pos = 0;
                        break;
                    }

                    break;
                }
    
                case waitLf:
                {
                    if (ch=='\r') // Повторный \r - засчитываем за перевод строки
                    {
                        ++line;
                        pos = 0;
                        break;
                    }

                    else if (ch=='\n')
                    {
                        ++line;
                        pos = 0;
                        st = waitStart;
                        break;
                    }

                    // Что-то другое - у нас перевод строки был единичным \r
                    ++line;
                    pos = 0;
                    goto explicit_waitStart;
                }
    
                case waitFirstTetrad:
                {
                    if (ch==' ')
                    {
                        if (allowSpaces) // keep st as waitStart
                        {
                            ++pos;
                            break;
                        }
                    }

                    else if (ch=='\r')
                    {
                        // process entry here
                        if (!curEntry.empty())
                        {
                            ParsingResult parseRes = ParsingResult::ok;
                            if (!curEntry.parseRawData(parseRes)) // Если что-то пошло не так, то мы получим false и в parseRes код возврата, его и возвращаем
                                return returnError(parseRes);
    
                            curEntry.lineNo = line;
                            resVec.emplace_back(curEntry.makeFitCopy());
                            curEntry.clear();
                        }

                        st = waitLf;
                        ++pos;

                        if (curEntry.isEof() && !allowMultiHex) // Очистка не стирает тип последней записи
                            return returnError(ParsingResult::ok);

                        break;
                    }

                    else if (ch=='\n')
                    {
                        // process entry here
                        if (!curEntry.empty())
                        {
                            ParsingResult parseRes = ParsingResult::ok;
                            if (!curEntry.parseRawData(parseRes))
                                return returnError(parseRes);
    
                            curEntry.lineNo = line;
                            resVec.emplace_back(curEntry.makeFitCopy());
                            curEntry.clear();
                        }

                        st = waitStart;
                        ++line;
                        pos = 0;

                        if (curEntry.isEof() && !allowMultiHex) // Очистка не стирает тип последней записи
                            return returnError(ParsingResult::ok);

                        break;
                    }

                    else
                    {
                        int d = utils::charToDigit(ch);
                        if (d<0)
                            return returnError(ParsingResult::notDigit); // Ждали цифру, пришла хрень
                        curByte = (std::uint8_t)(unsigned)d;
                        st = waitSecondTetrad;
                        ++pos;
                        break;
                    }

                    return returnError(ParsingResult::invalidRecord); // Что-то непонятное пришло
                }
    
                case waitSecondTetrad:
                {
                    int d = utils::charToDigit(ch);
                    if (d<0) // Ждали цифру
                    {
                        if (ch==' ' || ch=='\n' || ch=='\n')
                            return returnError(ParsingResult::brokenByte); // поймали пробел
                        else
                            return returnError(ParsingResult::notDigit); // пришла хрень
                    }

                    ++pos;
                    curByte <<= 4;
                    curByte |= (std::uint8_t)(unsigned)d;
                    curEntry.appendDataByte(curByte);
                    curByte = 0;
                    st = waitFirstTetrad;
                    break;
                }
    
            }
        
        }
    
        // Очистка не стирает тип последней записи, поэтому, если мы достигли конца данных, по хорошему предыдущая запись должна была быть EOF типа
        return returnError(curEntry.isEof() ? ParsingResult::ok : ParsingResult::unexpectedEnd );
    
    }

}; // class HexParser

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/marty_hex.h



