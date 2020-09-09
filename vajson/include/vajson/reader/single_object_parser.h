/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2020 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/**     \file       single_object_parser.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_SINGLE_OBJECT_PARSER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_SINGLE_OBJECT_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "vajson/reader/internal/level_validator.h"
#include "vajson/reader/strict_parser.h"

namespace vajson {
inline namespace reader {
/*!
 * \brief Provides a parser for a single object
 * \details Handles the OnStartObject, OnStartArray, OnEndObject, OnEndArray callbacks
 *          Calls a 'Finalize' callback on object end
 * \tparam Child The parser type which is called for every event that it implements
 *
 * \vpublic
 */
template <typename Child>
class SingleObjectParser : public StrictParser<Child> {
  /*!
   * \brief Unqualified access to the parent parser to appease a Doxygen parser error
   */
  using Parent = StrictParser<Child>;

 public:
  /*!
   * \brief constructor
   * \param doc The JSON document to parse
   */
  explicit SingleObjectParser(JsonDocument<char>& doc) noexcept : Parent{doc}, validator_{} {}

  /*!
   * \brief Default event for the start of objects
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   *
   * \vprivate
   */
  auto OnStartObject() -> Result<ParserState> { return this->validator_.Enter(); }

  /*!
   * \brief Default event for the end of objects
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   *
   * \vprivate
   */
  auto OnEndObject(std::size_t) -> Result<ParserState> {
    return this->validator_.Leave().Filter([this](ParserState) { return this->GetChild().Finalize(); });
  }

  /*!
   * \brief Default event for the start of arrays
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   *
   * \vprivate
   */
  static auto OnStartArray() -> Result<ParserState> {
    return Result<ParserState>::FromError(JsonErrc::kUserValidationFailed,
                                          "SingleObjectParser: Did not expect start of array.");
  }

  /*!
   * \brief Default event for the end of arrays
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   *
   * \vprivate
   */
  static auto OnEndArray(std::size_t) -> Result<ParserState> {
    return Result<ParserState>::FromError(JsonErrc::kUserValidationFailed,
                                          "SingleObjectParser: Did not expect end of array.");
  }

  /*!
   * \brief Abort parse if an unexpected event is encountered
   * \returns running an error
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, using default methods is not allowed}
   */
  static auto OnUnexpectedEvent() -> Result<ParserState> {
    return Result<ParserState>{
        MakeErrorCode(JsonErrc::kUserValidationFailed, "Expected to parse an object of elements.")};
  }

 private:
  /*!
   * \brief Validates if the only one level of object has been entered.
   */
  internal::LevelValidator validator_;
};
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_SINGLE_OBJECT_PARSER_H_
