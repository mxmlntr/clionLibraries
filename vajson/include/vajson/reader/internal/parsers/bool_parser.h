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
/**     \file       bool_parser.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_BOOL_PARSER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_BOOL_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "vajson/reader/parser.h"

namespace vajson {
inline namespace reader {
namespace internal {

/*!
 * \brief A parser that only parses a single boolean value.
 * \tparam Fn The function to be executed if the bool is read.
 */
template <typename Fn>
class BoolParser : public StrictParser<BoolParser<Fn>> {
  /*!
   * \brief Unqualified access to the parent parser to appease a Doxygen parser error
   */
  using Parent = StrictParser<BoolParser<Fn>>;

 public:
  /*!
   * \brief constructor
   * \param doc The JSON document to parse
   * \param fn The function to execute on bool
   */
  explicit BoolParser(JsonDocument<char>& doc, Fn&& fn) noexcept : Parent{doc}, fn_{std::forward<Fn>(fn)} {}

  /*!
   * \brief Event for Bools
   * \param v the boolean that is parsed
   * \returns The result of the callback passed to this class
   */
  auto OnBool(bool v) -> Result<ParserState> {
    static_assert(ara::core::is_result<std::result_of_t<Fn(bool)>>::value,
                  "TryBoolParser needs to return Result. Use BoolParser for void result.");
    return Result<void>{std::forward<Fn>(this->fn_)(v)}.Map([]() { return ParserState::kFinished; });
  }

  /*!
   * \brief Abort parse if an unexpected event is encountered
   * \returns running an error
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, using default methods is not allowed}
   */
  static auto OnUnexpectedEvent() -> ParserResult {
    return ParserResult{MakeErrorCode(JsonErrc::kUserValidationFailed, "Expected to parse a boolean.")};
  }

 private:
  /*!
   * \brief The function to be executed on the boolean value.
   */
  Fn fn_;
};
}  // namespace internal
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_BOOL_PARSER_H_
