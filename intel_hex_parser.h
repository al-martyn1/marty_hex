/*! \file
    \brief Hex file data parsing (Intel HEX)
 */

#pragma once

//----------------------------------------------------------------------------
#include "enums.h"
#include "file_pos_info.h"
#include "hex_entry.h"
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
class IntelHexParser
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
    // std::size_t line = 0;
    // std::size_t pos  = 0;
    FilePosInfo filePosInfo;
    HexInfo     hexInfo;

    void reset()
    {
        curEntry.reset();
        filePosInfo.line = 0;
        filePosInfo.pos  = 0;
        st = waitStart;
        hexInfo = HexInfo();
    }

    void setFileId(std::size_t fileId)
    {
        filePosInfo.file = fileId;
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
                     if (!curEntry.parseRawData(parseRes, &hexInfo)) // Если что-то пошло не так, то мы получим false и в parseRes код возврата, его и возвращаем
                         return parseRes;
         
                     curEntry.filePosInfo = filePosInfo;
                     resVec.emplace_back(curEntry.makeFitCopy());
                     curEntry.clear();
                 }
                 //return ParsingResult::notDigit;
                 return ParsingResult::unexpectedEnd;
            
            case waitSecondTetrad:
                 if (!curEntry.empty())
                 {
                     ParsingResult parseRes = ParsingResult::ok;
                     if (!curEntry.parseRawData(parseRes, &hexInfo)) // Если что-то пошло не так, то мы получим false и в parseRes код возврата, его и возвращаем
                         return parseRes;
         
                     curEntry.filePosInfo = filePosInfo;
                     resVec.emplace_back(curEntry.makeFitCopy());
                     curEntry.clear();
                 }
                 return ParsingResult::brokenByte;

            default:
                 return ParsingResult::invalidRecord;
        }
    }

// struct HexInfo
// {
//     std::uint32_t baseAddress = std::uint32_t(-1);
//     AddressMode   addressMode = AddressMode::none;
//  
// }; // struct HexInfo

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
                       ++filePosInfo.pos;
                       st = waitFirstTetrad;
                       break;
                    }

                    else if (ch=='#' || ch==';')
                    {
                        if (allowComments)
                        {
                            st = skipCommentLine;
                            ++filePosInfo.pos;
                            break;
                        }
                    }

                    else if (ch==' ')
                    {
                        if (allowSpaces) // keep st as waitStart
                        {
                            ++filePosInfo.pos;
                            break;
                        }
                    }

                    else if (ch=='\r')
                    {
                        st = waitLf;
                        ++filePosInfo.pos;
                        break;
                    }

                    else if (ch=='\n')
                    {
                        ++filePosInfo.line;
                        filePosInfo.pos = 0;
                        break;
                    }

                    else if (ch==0x1A) // Ctrl+Z/EOF
                    {
                         return returnError(curEntry.isEof() ? ParsingResult::ok : ParsingResult::unexpectedEnd );
                    }

                    return returnError(ParsingResult::invalidRecord); // Что-то непонятное пришло
                }
    
                case skipCommentLine:
                {
                    ++filePosInfo.pos;

                    if (ch=='\r')
                    {
                        st = waitLf;
                        break;
                    }

                    else if (ch=='\n')
                    {
                        ++filePosInfo.line;
                        filePosInfo.pos = 0;
                        break;
                    }

                    break;
                }
    
                case waitLf:
                {
                    if (ch=='\r') // Повторный \r - засчитываем за перевод строки
                    {
                        ++filePosInfo.line;
                        filePosInfo.pos = 0;
                        break;
                    }

                    else if (ch=='\n')
                    {
                        ++filePosInfo.line;
                        filePosInfo.pos = 0;
                        st = waitStart;
                        break;
                    }

                    // Что-то другое - у нас перевод строки был единичным \r
                    ++filePosInfo.line;
                    filePosInfo.pos = 0;
                    goto explicit_waitStart;
                }
    
                case waitFirstTetrad:
                {
                    if (ch==' ')
                    {
                        if (allowSpaces) // keep st as waitStart
                        {
                            ++filePosInfo.pos;
                            break;
                        }
                    }

                    else if (ch=='\r')
                    {
                        // process entry here
                        if (!curEntry.empty())
                        {
                            ParsingResult parseRes = ParsingResult::ok;
                            if (!curEntry.parseRawData(parseRes, &hexInfo)) // Если что-то пошло не так, то мы получим false и в parseRes код возврата, его и возвращаем
                                return returnError(parseRes);
    
                            curEntry.filePosInfo = filePosInfo;
                            resVec.emplace_back(curEntry.makeFitCopy());
                            curEntry.clear();
                        }

                        st = waitLf;
                        ++filePosInfo.pos;

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
                            if (!curEntry.parseRawData(parseRes, &hexInfo))
                                return returnError(parseRes);
    
                            curEntry.filePosInfo = filePosInfo;
                            resVec.emplace_back(curEntry.makeFitCopy());
                            curEntry.clear();
                        }

                        st = waitStart;
                        ++filePosInfo.line;
                        filePosInfo.pos = 0;

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
                        ++filePosInfo.pos;
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

                    ++filePosInfo.pos;
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

}; // class IntelHexParser

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------

} // namespace hex
} // namespace marty
// marty::hex::
// marty_hex/marty_hex.h



