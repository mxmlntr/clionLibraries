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
/**     \file       string_parser.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_STRING_PARSER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_STRING_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <string>
#include <utility>

#include "vajson/reader/parser.h"

namespace vajson {
inline namespace reader {
namespace internal {

/*!
 * \brief A parser that only parses a single string value.
 * \tparam Fn The function to be executed if the string is read and returns a result.
 */
template <typename Fn>
class StringParser : public StrictParser<StringParser<Fn>> {
  /*!
   * \brief Unqualified access to the parent parser to appease a Doxygen parser error
   */
  using Parent = StrictParser<StringParser<Fn>>;

 public:
  /*!
   * \brief constructor
   * \param doc The JSON document to parse
   * \param fn The function to execute on the string
   */
  explicit StringParser(JsonDocument<char>& doc, Fn&& fn) noexcept : Parent{doc}, fn_{std::forward<Fn>(fn)} {}

  /*!
   * \brief Event for Strings
   * \param string The string that is parsed
   * \returns The result of the callback passed to this class
   */
  auto OnString(vac::container::CStringView string) -> Result<ParserState> {
    static_assert(ara::core::is_result<std::result_of_t<Fn(vac::container::CStringView)>>::value,
                  "TryParseString needs to return Result. Use ParseString for void result.");
    return this->fn_(string).Map([]() { return ParserState::kFinished; });
  }

  /*!
   * \brief Abort parse if an unexpected event is encountered
   * \returns running an error
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, using default methods is not allowed}
   */
  static auto OnUnexpectedEvent() -> ParserResult {
    return ParserResult{MakeErrorCode(JsonErrc::kUserValidationFailed, "Expected to parse a string.")};
  }

 private:
  /*!
   * \brief The function to be executed on the string value.
   */
  Fn fn_;
};

}  // namespace internal
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_STRING_PARSER_H_
