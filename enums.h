/*! \file
    \brief Autogenerated enumerations (Umba Enum Gen)
 */

#pragma once


#include "marty_cpp/marty_enum.h"
#include "marty_cpp/marty_flag_ops.h"
#include "marty_cpp/marty_flags.h"

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>



namespace marty{
namespace hex{

//#!HexRecordType
enum class HexRecordType : std::uint8_t
{
    invalid                  = (std::uint8_t)(-1) /*!<  */,
    unknown                  = (std::uint8_t)(-1) /*!<  */,
    data                     = 0x00 /*!< Data */,
    eof                      = 0x01 /*!< End Of File. Must occur exactly once per file in the last record of the file. */,
    extendedSegmentAddress   = 0x02 /*!< Extended Segment Address */,
    startSegmentAddress      = 0x03 /*!< Start Segment Address */,
    extendedLinearAddress    = 0x04 /*!< Extended Linear Address */,
    startLinearAddress       = 0x05 /*!< Start Linear Address */

}; // enum 
//#!

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(HexRecordType)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( HexRecordType, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HexRecordType::startLinearAddress       , "StartLinearAddress"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HexRecordType::startSegmentAddress      , "StartSegmentAddress"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HexRecordType::extendedSegmentAddress   , "ExtendedSegmentAddress" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HexRecordType::extendedLinearAddress    , "ExtendedLinearAddress"  );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HexRecordType::eof                      , "Eof"                    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HexRecordType::data                     , "Data"                   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HexRecordType::invalid                  , "Invalid"                );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( HexRecordType, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( HexRecordType, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::startLinearAddress       , "start-linear-address"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::startLinearAddress       , "start_linear_address"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::startLinearAddress       , "startlinearaddress"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::startSegmentAddress      , "start-segment-address"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::startSegmentAddress      , "start_segment_address"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::startSegmentAddress      , "startsegmentaddress"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::extendedSegmentAddress   , "extended-segment-address" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::extendedSegmentAddress   , "extended_segment_address" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::extendedSegmentAddress   , "extendedsegmentaddress"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::extendedLinearAddress    , "extended-linear-address"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::extendedLinearAddress    , "extended_linear_address"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::extendedLinearAddress    , "extendedlinearaddress"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::eof                      , "eof"                      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::data                     , "data"                     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::invalid                  , "unknown"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HexRecordType::invalid                  , "invalid"                  );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( HexRecordType, std::map, 1 )


//#!ParsingResult
enum class ParsingResult : std::uint32_t
{
    ok                           = 0x00 /*!< EOF record reached (not all data may be parsed) */,
    eof                          = 0x00 /*!< EOF record reached (not all data may be parsed) */,
    unexpectedEnd                = 0x01 /*!< End of data encountered, but no EOF record found */,
    unexpectedSpace              = 0x02 /*!< Unexpected space character encountered */,
    invalidRecord                = 0x03 /*!< The Record must begin with the 'colon' character */,
    unknownRecordType            = 0x04 /*!< Found a record with unknown type */,
    notDigit                     = 0x05 /*!< Non-xdigit character encountered */,
    brokenByte                   = 0x06 /*!< Bytes are represented eith pairs of hex digits. Appearance of a single digit mean that byte is broken */,
    tooManyDataBytes             = 0x07 /*!< Number of data bytes reached is greater than was stated at record start */,
    tooFewDataBytes              = 0x08 /*!< Number of data bytes reached is less than was stated at record start */,
    tooFewBytes                  = 0x09 /*!< Too few bytes in record */,
    checksumMismatch             = 0x0A /*!< Checksum mismatch */,
    dataSizeNotMatchRecordType   = 0x0B /*!< Number of data bytes in record does not match the record type */,
    invalidArgument              = 0x0C /*!< One, some, or all passed arguments are invalid */,
    mismatchAddressMode          = 0x0D /*!< Address mode mismatch to previously assigned address mode (mixed segment and linear address records) */,
    mismatchStartAddressMode     = 0x0E /*!< Start address mode mismatch to address mode (mixed segment and linear address records) */,
    multipleStartAddress         = 0x0F /*!< Start address already defined */,
    memoryOverlaps               = 0x10 /*!< Multiple records adress the same memory */

}; // enum 
//#!

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(ParsingResult)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( ParsingResult, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::memoryOverlaps               , "MemoryOverlaps"             );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::multipleStartAddress         , "MultipleStartAddress"       );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::mismatchStartAddressMode     , "MismatchStartAddressMode"   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::tooFewBytes                  , "TooFewBytes"                );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::invalidArgument              , "InvalidArgument"            );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::unexpectedEnd                , "UnexpectedEnd"              );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::mismatchAddressMode          , "MismatchAddressMode"        );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::unexpectedSpace              , "UnexpectedSpace"            );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::unknownRecordType            , "UnknownRecordType"          );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::invalidRecord                , "InvalidRecord"              );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::notDigit                     , "NotDigit"                   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::dataSizeNotMatchRecordType   , "DataSizeNotMatchRecordType" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::tooManyDataBytes             , "TooManyDataBytes"           );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::tooFewDataBytes              , "TooFewDataBytes"            );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::ok                           , "Ok"                         );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::brokenByte                   , "BrokenByte"                 );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ParsingResult::checksumMismatch             , "ChecksumMismatch"           );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( ParsingResult, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( ParsingResult, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::memoryOverlaps               , "memory-overlaps"                 );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::memoryOverlaps               , "memory_overlaps"                 );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::memoryOverlaps               , "memoryoverlaps"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::multipleStartAddress         , "multiple-start-address"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::multipleStartAddress         , "multiple_start_address"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::multipleStartAddress         , "multiplestartaddress"            );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::mismatchStartAddressMode     , "mismatch-start-address-mode"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::mismatchStartAddressMode     , "mismatch_start_address_mode"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::mismatchStartAddressMode     , "mismatchstartaddressmode"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::tooFewBytes                  , "too-few-bytes"                   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::tooFewBytes                  , "too_few_bytes"                   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::tooFewBytes                  , "toofewbytes"                     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::invalidArgument              , "invalid-argument"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::invalidArgument              , "invalid_argument"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::invalidArgument              , "invalidargument"                 );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::unexpectedEnd                , "unexpected-end"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::unexpectedEnd                , "unexpected_end"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::unexpectedEnd                , "unexpectedend"                   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::mismatchAddressMode          , "mismatch-address-mode"           );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::mismatchAddressMode          , "mismatch_address_mode"           );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::mismatchAddressMode          , "mismatchaddressmode"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::unexpectedSpace              , "unexpected-space"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::unexpectedSpace              , "unexpectedspace"                 );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::unexpectedSpace              , "unexpected_space"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::unknownRecordType            , "unknown-record-type"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::unknownRecordType            , "unknown_record_type"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::unknownRecordType            , "unknownrecordtype"               );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::invalidRecord                , "invalid-record"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::invalidRecord                , "invalid_record"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::invalidRecord                , "invalidrecord"                   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::notDigit                     , "not-digit"                       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::notDigit                     , "not_digit"                       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::notDigit                     , "notdigit"                        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::dataSizeNotMatchRecordType   , "data-size-not-match-record-type" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::dataSizeNotMatchRecordType   , "data_size_not_match_record_type" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::dataSizeNotMatchRecordType   , "datasizenotmatchrecordtype"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::tooManyDataBytes             , "too-many-data-bytes"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::tooManyDataBytes             , "too_many_data_bytes"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::tooManyDataBytes             , "toomanydatabytes"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::tooFewDataBytes              , "too-few-data-bytes"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::tooFewDataBytes              , "too_few_data_bytes"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::tooFewDataBytes              , "toofewdatabytes"                 );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::ok                           , "eof"                             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::ok                           , "ok"                              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::brokenByte                   , "broken-byte"                     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::brokenByte                   , "broken_byte"                     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::brokenByte                   , "brokenbyte"                      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::checksumMismatch             , "checksum-mismatch"               );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::checksumMismatch             , "checksum_mismatch"               );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ParsingResult::checksumMismatch             , "checksummismatch"                );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( ParsingResult, std::map, 1 )


//#!AddressMode
enum class AddressMode : std::uint32_t
{
    none                 = 0x00 /*!<  */,
    undefined            = 0x00 /*!<  */,
    sba                  = 0x01 /*!< Segment Base Address */,
    segmentBaseAddress   = 0x01 /*!< Segment Base Address */,
    lba                  = 0x02 /*!< Linear Base Address */,
    linearBaseAddress    = 0x02 /*!< Linear Base Address */

}; // enum 
//#!

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(AddressMode)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( AddressMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( AddressMode::lba    , "Lba"  );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( AddressMode::sba    , "Sba"  );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( AddressMode::none   , "None" );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( AddressMode, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( AddressMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::lba    , "linear-base-address"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::lba    , "linearbaseaddress"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::lba    , "linear_base_address"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::lba    , "lba"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::sba    , "segment-base-address" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::sba    , "segment_base_address" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::sba    , "segmentbaseaddress"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::sba    , "sba"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::none   , "undefined"            );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( AddressMode::none   , "none"                 );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( AddressMode, std::map, 1 )


//#!ParsingOptions
enum class ParsingOptions : std::uint32_t
{
    none            = 0x00 /*!<  */,
    allowComments   = 0x01 /*!< Allow comments (lines with '#' character first) */,
    allowSpaces     = 0x02 /*!< Allow spaces in HEX lines */,
    allowMultiHex   = 0x04 /*!< Normal HEX ends with EOF record. If we need read multiple HEXes from single text, we set this option */

}; // enum 
//#!

MARTY_CPP_MAKE_ENUM_FLAGS(ParsingOptions)

MARTY_CPP_ENUM_FLAGS_SERIALIZE_BEGIN( ParsingOptions, std::map, 1 )
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParsingOptions::allowMultiHex   , "AllowMultiHex" );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParsingOptions::allowSpaces     , "AllowSpaces"   );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParsingOptions::allowComments   , "AllowComments" );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParsingOptions::none            , "None"          );
MARTY_CPP_ENUM_FLAGS_SERIALIZE_END( ParsingOptions, std::map, 1 )

MARTY_CPP_ENUM_FLAGS_DESERIALIZE_BEGIN( ParsingOptions, std::map, 1 )
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::allowMultiHex   , "allow-multi-hex" );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::allowMultiHex   , "allow_multi_hex" );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::allowMultiHex   , "allowmultihex"   );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::allowSpaces     , "allow-spaces"    );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::allowSpaces     , "allow_spaces"    );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::allowSpaces     , "allowspaces"     );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::allowComments   , "allow-comments"  );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::allowComments   , "allow_comments"  );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::allowComments   , "allowcomments"   );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParsingOptions::none            , "none"            );
MARTY_CPP_ENUM_FLAGS_DESERIALIZE_END( ParsingOptions, std::map, 1 )


//#!HexRecordsCheckCode
enum class HexRecordsCheckCode : std::uint32_t
{
    none                       = 0x00 /*!<  */,
    memoryOverlaps             = 0x01 /*!< Multiple records adress the same memory */,
    mismatchAddressMode        = 0x02 /*!< Address mode mismatch to previously assigned address mode (mixed segment and linear address records) */,
    mismatchStartAddressMode   = 0x04 /*!< Start address mode mismatch to address mode (mixed segment and linear address records) */

}; // enum 
//#!

MARTY_CPP_MAKE_ENUM_FLAGS(HexRecordsCheckCode)

MARTY_CPP_ENUM_FLAGS_SERIALIZE_BEGIN( HexRecordsCheckCode, std::map, 1 )
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( HexRecordsCheckCode::mismatchStartAddressMode   , "MismatchStartAddressMode" );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( HexRecordsCheckCode::mismatchAddressMode        , "MismatchAddressMode"      );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( HexRecordsCheckCode::memoryOverlaps             , "MemoryOverlaps"           );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( HexRecordsCheckCode::none                       , "None"                     );
MARTY_CPP_ENUM_FLAGS_SERIALIZE_END( HexRecordsCheckCode, std::map, 1 )

MARTY_CPP_ENUM_FLAGS_DESERIALIZE_BEGIN( HexRecordsCheckCode, std::map, 1 )
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::mismatchStartAddressMode   , "mismatch-start-address-mode" );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::mismatchStartAddressMode   , "mismatch_start_address_mode" );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::mismatchStartAddressMode   , "mismatchstartaddressmode"    );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::mismatchAddressMode        , "mismatch-address-mode"       );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::mismatchAddressMode        , "mismatch_address_mode"       );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::mismatchAddressMode        , "mismatchaddressmode"         );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::memoryOverlaps             , "memory-overlaps"             );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::memoryOverlaps             , "memory_overlaps"             );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::memoryOverlaps             , "memoryoverlaps"              );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( HexRecordsCheckCode::none                       , "none"                        );
MARTY_CPP_ENUM_FLAGS_DESERIALIZE_END( HexRecordsCheckCode, std::map, 1 )

} // namespace hex
} // namespace marty

