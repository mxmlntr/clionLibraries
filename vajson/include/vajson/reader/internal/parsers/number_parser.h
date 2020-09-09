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
/**     \file       number_parser.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_NUMBER_PARSER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_NUMBER_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "vajson/reader/parser.h"

namespace vajson {
inline namespace reader {
namespace internal {

/*!
 * \brief A parser that only parses a single number value.
 * \tparam T The number type to parse
 * \tparam Fn The function to be executed if the number is read.
 */
template <typename T, typename Fn>
class NumberParser : public StrictParser<NumberParser<T, Fn>> {
  /*!
   * \brief Unqualified access to the parent parser to appease a Doxygen parser error
   */
  using Parent = StrictParser<NumberParser<T, Fn>>;

 public:
  /*!
   * \brief constructor
   * \param doc The JSON document to parse
   * \param fn The function to execute on the number
   */
  explicit NumberParser(JsonData& doc, Fn&& fn) noexcept : Parent{doc}, fn_{std::forward<Fn>(fn)} {}

  /*!
   * \brief Event for Numbers
   * \param number The number that is parsed
   * \returns The result of the callback passed to this class
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if the number could not be converted to the wanted type.}
   */
  auto OnNumber(util::JsonNumber number) -> Result<ParserState> {
    static_assert(ara::core::is_result<std::result_of_t<Fn(T)>>::value,
                  "TryParseNumber needs to return Result. Use ParseNumber for void result.");
    return number.TryAs<T>().AndThen(std::forward<Fn>(this->fn_)).Map([]() { return ParserState::kFinished; });
  }

  /*!
   * \brief Abort parse if an unexpected event is encountered
   * \returns running an error
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, using default methods is not allowed}
   */
  static auto OnUnexpectedEvent() -> ParserResult {
    return ParserResult{MakeErrorCode(JsonErrc::kUserValidationFailed, "Expected to parse a number.")};
  }

 private:
  /*!
   * \brief The function to be executed on the number value.
   */
  Fn fn_;
};
}  // namespace internal
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_NUMBER_PARSER_H_
