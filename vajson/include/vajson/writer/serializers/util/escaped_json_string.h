/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2019 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/*!        \file  escaped_json_string.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_UTIL_ESCAPED_JSON_STRING_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_UTIL_ESCAPED_JSON_STRING_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <string>
#include <utility>

#include "ara/core/string_view.h"
#include "vac/container/string_literals.h"

#include "vajson/util/types.h"
#include "vajson/writer/serializers/structures/serializer.h"
#include "vajson/writer/serializers/util/literals.h"
#include "vajson/writer/types/basic_types.h"

namespace vajson {
namespace writer {
inline namespace serializers {
namespace internal {

/*!
 * \brief Provides the possibility to write valid JSON string literals
 * \tparam Char The character type
 */
class EscapedJsonString {
 public:
  /*!
   * \brief Constructor from a key
   * \param key The key to serialize
   */
  explicit EscapedJsonString(JKeyType key) noexcept : value_(key.GetValue()) {}
  /*!
   * \brief Constructor from a string
   * \param string The string to serialize
   */
  explicit EscapedJsonString(JStringType string) noexcept : value_(string.GetValue()) {}

  /*!
   * \brief Returns the contained value
   * \return the contained value
   */
  vac::container::string_view GetValue() const { return this->value_; }

 private:
  /*!
   * \brief The value to write as a JSON string literal
   */
  vac::container::string_view value_;
};

/*!
 * \brief Writes a non-escaped string literal to JSON
 * \param os the output stream to write into
 * \param string the value of the string to escape & write
 * \return the output stream to enable chaining
 */
auto inline operator<<(std::ostream& os, EscapedJsonString string) noexcept -> std::ostream& {
  for (char8_t ch : string.GetValue()) {
    switch (ch) {
      case '"': {
        os << kEscapedQuote;
        break;
      }
      case '\\': {
        os << kBackslash;
        break;
      }
      case '/': {
        os << kSlash;
        break;
      }
      case '\b': {
        os << kBackSpace;
        break;
      }
      case '\f': {
        os << kFormFeed;
        break;
      }
      case '\n': {
        os << kNewline;
        break;
      }
      case '\r': {
        os << kCarriageReturn;
        break;
      }
      case '\t': {
        os << kTab;
        break;
      }
      default:
        os.put(ch);
        break;
    }
  }

  return os;
}
}  // namespace internal
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_UTIL_ESCAPED_JSON_STRING_H_
