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
/**     \file       array_parser.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_ARRAY_PARSER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_ARRAY_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "vajson/reader/single_array_parser.h"

namespace vajson {
inline namespace reader {
namespace internal {

/*!
 * \brief A parser that only parses multiple values of the same type.
 * \tparam Fn The function to be executed when the elements are read.
 */
template <typename Fn>
class ArrayParser : public SingleArrayParser<ArrayParser<Fn>> {
  /*!
   * \brief Unqualified access to the parent parser to appease a Doxygen parser error
   */
  using Parent = SingleArrayParser<ArrayParser<Fn>>;

 public:
  /*!
   * \brief constructor
   * \param doc The JSON document to parse
   * \param fn The function to execute on the numbers
   */
  explicit ArrayParser(JsonDocument<char>& doc, Fn&& fn) noexcept : Parent{doc}, fn_{std::forward<Fn>(fn)} {}

  /*!
   * \brief Event for Elements
   * \returns The result of the callback passed to this class
   */
  auto OnElement() -> Result<ParserState> {
    return std::forward<Fn>(this->fn_)(this->GetIndex()).Map([this]() { return ParserState::kRunning; });
  }

 private:
  Fn fn_; /*!< The function to be executed on the numbers value. */
};

}  // namespace internal
}  // namespace reader
}  // namespace vajson
#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_PARSERS_ARRAY_PARSER_H_
