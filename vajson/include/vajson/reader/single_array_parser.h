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
/**     \file       single_array_parser.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_SINGLE_ARRAY_PARSER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_SINGLE_ARRAY_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "vajson/reader/internal/level_validator.h"
#include "vajson/reader/strict_parser.h"

namespace vajson {
inline namespace reader {
template <typename Child>
class StrictParser;

/*!
 * \brief Provides a parser for a single array
 * \details Handles the OnStartObject, OnStartArray, OnEndObject, OnEndArray & OnComma callbacks
 *          Calls a 'Finalize' callback on array end
 * \tparam Child The parser type which is called for every event that it implements
 *
 * \vpublic
 */
template <typename Child>
class SingleArrayParser : public StrictParser<Child> {
  /*!
   * \brief Unqualified access to the parent parser to appease a Doxygen parser error
   */
  using Parent = StrictParser<Child>;

 public:
  /*!
   * \brief constructor
   * \param doc The JSON document to parse
   */
  explicit SingleArrayParser(JsonDocument<char>& doc) noexcept : Parent{doc}, validator_{}, index_{0} {}

  /*!
   * \brief Default event for the start of objects
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   *
   * \vprivate
   */
  static auto OnStartObject() -> Result<ParserState> {
    return Result<ParserState>::FromError(JsonErrc::kUserValidationFailed,
                                          "SingleArrayParser: Did not expect start of object.");
  }

  /*!
   * \brief Default event for the end of objects
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   *
   * \vprivate
   */
  static auto OnEndObject(std::size_t) -> Result<ParserState> {
    return Result<ParserState>::FromError(JsonErrc::kUserValidationFailed,
                                          "SingleArrayParser: Did not expect end of object.");
  }

  /*!
   * \brief Default event for the start of arrays
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   *
   * \vprivate
   */
  auto OnStartArray() -> Result<ParserState> {
    return this->validator_.Enter().AndThen([this](ParserState) { return ProcessElement(); });
  }

  /*!
   * \brief Default event for every comma that is parsed
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   *
   * \vprivate
   */
  auto OnComma() -> Result<ParserState> { return ProcessElement(); }

  /*!
   * \brief Default event for the end of arrays
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   *
   * \vprivate
   */
  auto OnEndArray(std::size_t) -> Result<ParserState> {
    return this->validator_.Leave().Filter([this](ParserState) { return this->GetChild().Finalize(); });
  }

  /*!
   * \brief Abort parse if an unexpected event is encountered
   * \returns running an error
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, using default methods is not allowed}
   */
  static auto OnUnexpectedEvent() -> Result<ParserState> {
    return Result<ParserState>{
        MakeErrorCode(JsonErrc::kUserValidationFailed, "Expected to parse an array of elements.")};
  }

  /*!
   * \brief Returns the index for the current element
   * \return The index for the current element
   *
   * \vpublic
   */
  auto GetIndex() const -> std::size_t { return this->index_; }

 protected:
  /*!
   * \brief Can be used to validate parser results or fill in inout parameters etc.
   * \details Default implementation does nothing
   * \return Nothing on success. Else an error code defined by the application parser.
   */
  static auto Finalize() -> ara::core::Result<void> { return ara::core::Result<void>{}; }

 private:
  /*!
   * \brief Processes each element
   * \return the callback result
   */
  auto ProcessElement() -> Result<ParserState> {
    Result<ParserState> result{this->GetChild().OnElement()};
    index_ += 1;
    return result;
  }

  /*!
   * \brief Validates if the only one level of array has been entered.
   */
  internal::LevelValidator validator_;

  /*!
   * \brief Index for the array elements
   */
  std::size_t index_;
};
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_SINGLE_ARRAY_PARSER_H_
