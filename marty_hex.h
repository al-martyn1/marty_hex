/*! \file
    \brief Hex file data parsing (Intel HEX)
 */

#pragma once

#include "enums.h"
#include "utils.h"


#include <string>
#include <cstdint>
#include <vector>


// marty_hex/marty_hex.h
// marty::hex::
namespace marty{
namespace hex{


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

using byte_string = std::basic_string<std::uint8_t>;

//! При разборе сначала все байты кладутся в массив data, и только по окончании строки производится разбор на составляющие (перед этим проверяется КС)
struct HexEntry
{
    std::uint8_t      numDataBytes = 0;
    std::uint16_t     address      = 0;
    HexRecordType     recordType   = HexRecordType::eof;
    byte_string       data;
    std::uint8_t      checksum     = 0;

    void reset()
    {
        numDataBytes = 0;
        address      = 0;
        recordType   = HexRecordType::eof;
        data.clear();
        checksum     = 0;
    }

    void clear() { reset(); }

    bool empty() const
    {
        return numDataBytes==0 && address==0 && recordType==HexRecordType::eof && data.empty() && checksum==0;
    }

    void appendDataByte(std::uint8_t b)
    {
        data.append(1, b);
    }

    HexEntry makeFitCopy() const
    {
        HexEntry res = *this;
        //res.data.shrink_to_fit();
        byte_string dataTmp = byte_string(res.data.begin(), res.data.end());
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
    
        return (std::uint8_t)0 - csum;
    }

    bool parseRawData(ParsingResult &r)
    {
        if (data.size()<5)
            return r=ParsingResult::tooFewBytes, false;

        std::uint8_t csum = calcChecksum(data.data(), data.size()-1);
        //std::uint8_t csumReaded = data.back(); // back что-то вроде не то возвращает, но может, какой-то мой косяк
        std::uint8_t csumReaded = data[data.size()-1]; // берём по индексу
        if (csum!=csumReaded)
            return r=ParsingResult::checksumMismatch, false;

        checksum = csum;

        numDataBytes = data[0];

        address = (std::uint16_t)data[1];
        address <<= 8;
        address |= (std::uint16_t)data[2];

        recordType = (HexRecordType)data[3];

        data.erase(0,4);

        if (data.size()>(std::size_t)numDataBytes)
            return r=ParsingResult::tooManyDataBytes, false;

        if (data.size()<(std::size_t)numDataBytes)
            return r=ParsingResult::tooFewDataBytes, false;

        // Проверяем количество байт данных типу записи
        switch(recordType)
        {
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

    // TODO: написать finalize

    ParsingResult parseTextChunk( std::vector<HexEntry> &resVec
                                , const std::string &hexText
                                , std::size_t startIdx = 0
                                , std::size_t *pErrorOffset=0
                                , ParsingOptions parsingOptions = ParsingOptions::none
                                )
    {
        return parseTextChunk(resVec, hexText.data(), hexText.size(), startIdx, pErrorOffset, parsingOptions);
    }

    ParsingResult parseTextChunk( std::vector<HexEntry> &resVec
                                , const char* pData     // ptr to text chunk start
                                , std::size_t size      // text chunk start
                                , std::size_t startIdx = 0
                                , std::size_t *pErrorOffset=0
                                , ParsingOptions parsingOptions = ParsingOptions::none
                                )
    {
        std::size_t  idx     = startIdx;
        std::uint8_t curByte = 0;

        bool allowComments = (parsingOptions&ParsingOptions::allowComments)!=0;
        bool allowSpaces   = (parsingOptions&ParsingOptions::allowSpaces  )!=0;

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
                            if (!curEntry.parseRawData(parseRes))
                                return returnError(parseRes);
    
                            resVec.emplace_back(curEntry.makeFitCopy());
                            curEntry.clear();
                        }

                        st = waitLf;
                        ++pos;
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
    
                            resVec.emplace_back(curEntry.makeFitCopy());
                            curEntry.clear();
                        }

                        st = waitStart;
                        ++line;
                        pos = 0;
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
                    curByte = (std::uint8_t)(unsigned)d;
                    curEntry.appendDataByte(curByte);
                    curByte = 0;
                    st = waitFirstTetrad;
                    break;
                }
    
            }
        
        }
    
        return returnError(ParsingResult::unexpectedEnd);
    
    }



}; // class HexParser








} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/marty_hex.h



