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
/**     \file       strict_parser.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_STRICT_PARSER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_STRICT_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "vajson/reader/internal/parsers/array_parser.h"
#include "vajson/reader/internal/parsers/bool_parser.h"
#include "vajson/reader/internal/parsers/key_parser.h"
#include "vajson/reader/internal/parsers/number_parser.h"
#include "vajson/reader/internal/parsers/string_parser.h"
#include "vajson/reader/parser.h"

namespace vajson {
inline namespace reader {
namespace internal {
/*!
 * \brief Returns bool in case the function returns any Result
 */
template <typename F, typename T>
using ReturnsResult = ara::core::is_result<std::result_of_t<F(T const&)>>;
/*!
 * \brief Returns bool in case the function returns a Result<void>
 */
template <typename F, typename T>
using ReturnsResultVoid = std::is_same<Result<void>, std::result_of_t<F(T const&)>>;
/*!
 * \brief Returns bool in case the function returns void
 */
template <typename F, typename T>
using ReturnsVoid = std::is_void<std::result_of_t<F(T const&)>>;

/*!
 * \brief  SFINAE for callable returning Result<...>.
 * \tparam F Callable.
 * \vprivate
 */
template <typename F, typename T, typename Out>
using CallableReturnsResult = std::enable_if_t<ReturnsResult<F, T>::value, Out>;

/*!
 * \brief  SFINAE for callable returning Result<...>.
 * \tparam F Callable.
 * \vprivate
 */
template <typename F, typename T, typename Out>
using CallableReturnsNoResult = std::enable_if_t<!ReturnsResult<F, T>::value, Out>;

/*!
 * \brief Returns bool in case the function returns any Result
 */
template <typename F, typename T>
using ArrayReturnsResult = ara::core::is_result<std::result_of_t<F(std::size_t, T const&)>>;
/*!
 * \brief Returns bool in case the function returns a Result<void>
 */
template <typename F, typename T>
using ArrayReturnsResultVoid = std::is_same<Result<void>, std::result_of_t<F(std::size_t, T const&)>>;
/*!
 * \brief Returns bool in case the function returns void
 */
template <typename F, typename T>
using ArrayReturnsVoid = std::is_void<std::result_of_t<F(std::size_t, T const&)>>;

/*!
 * \brief  SFINAE for callable returning Result<...>.
 * \tparam F Callable.
 * \vprivate
 */
template <typename F, typename T, typename Out>
using ArrayCallableReturnsResult = std::enable_if_t<ArrayReturnsResult<F, T>::value, Out>;

/*!
 * \brief  SFINAE for callable returning Result<...>.
 * \tparam F Callable.
 * \vprivate
 */
template <typename F, typename T, typename Out>
using ArrayCallableReturnsNoResult = std::enable_if_t<!ArrayReturnsResult<F, T>::value, Out>;

}  // namespace internal

/*!
 * \brief A parser that aborts on unknown values
 * \tparam Child The parser type which is called for every event that it implements
 *
 * \vpublic
 */
template <typename Child>
class StrictParser : public Parser<Child> {
  /*!
   * Shorthand for the Result
   */
  using R = Result<ParserState>;

 public:
  /*!
   * \brief constructor
   * \param doc The JSON document to parse
   */
  explicit StrictParser(JsonDocument<char>& doc) noexcept : Parser<Child>{doc} {}

  /*!
   * \brief Abort parse if an unexpected event is encountered
   * \returns running an error
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, using default methods is not allowed}
   */
  static auto OnUnexpectedEvent() -> R {
    return R{MakeErrorCode(JsonErrc::kUserValidationFailed, "Use of default method not allowed in this context.")};
  }

  /*!
   * \brief Parses the following key value and fails if no key comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on keys
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseKey(Fn&& fn) -> internal::CallableReturnsNoResult<Fn, vac::container::CStringView, R> {
    static_assert(internal::ReturnsVoid<Fn, vac::container::CStringView>::value, "Must return void");
    return TryParseKey([&fn](vac::container::CStringView s) {
      std::forward<Fn>(fn)(s);
      return Result<void>{};
    });
  }

  /*!
   * \brief Parses the following key value and fails if no key comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on keys
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseKey(Fn&& fn) -> internal::CallableReturnsResult<Fn, vac::container::CStringView, R> {
    static_assert(internal::ReturnsResultVoid<Fn, vac::container::CStringView>::value, "Must return Result<void>");
    return internal::KeyParser<Fn>(this->GetJsonDocument(), std::forward<Fn>(fn)).SubParse();
  }

  /*!
   * \brief Checks if the next element is a key with value key
   * \param key The key to compare with
   * \return Running if the keys are the same
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if an unexpected key is read.}
   */
  auto CheckKey(vac::container::CStringView key) -> R {
    return ParseKey([&key](vac::container::CStringView s) {
      Result<void> result{};
      if (key != s) {
        result.EmplaceError(JsonErrc::kUserValidationFailed, "Incorrect key received");
      }
      return result;
    });
  }

  /*!
   * \brief Parses the following bool value and fails if no bool comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on bool
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseBool(Fn&& fn) -> internal::CallableReturnsNoResult<Fn, bool, R> {
    static_assert(internal::ReturnsVoid<Fn, bool>::value, "Must return void");
    return ParseBool([&fn](bool b) {
      std::forward<Fn>(fn)(b);
      return Result<void>{};
    });
  }

  /*!
   * \brief Parses the following bool value and fails if no bool comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on bool
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseBool(Fn&& fn) -> internal::CallableReturnsResult<Fn, bool, R> {
    static_assert(internal::ReturnsResultVoid<Fn, bool>::value, "Must return Result<void>");
    return internal::BoolParser<Fn>(this->GetJsonDocument(), std::forward<Fn>(fn)).SubParse();
  }

  /*!
   * \brief Parses the following number value and fails if no number comes next.
   * \tparam T The number type to parse
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on numbers
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename T, typename Fn>
  auto ParseNumber(Fn&& fn) -> internal::CallableReturnsNoResult<Fn, T, R> {
    static_assert(internal::ReturnsVoid<Fn, T>::value, "Must return void");
    return ParseNumber<T>([&fn](T n) {
      std::forward<Fn>(fn)(n);
      return Result<void>{};
    });
  }

  /*!
   * \brief Parses the following number value and fails if no number comes next.
   * \tparam T The number type to parse
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on numbers
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename T, typename Fn>
  auto ParseNumber(Fn&& fn) -> internal::CallableReturnsResult<Fn, T, R> {
    static_assert(internal::ReturnsResultVoid<Fn, T>::value, "Must return Result<void>");
    return internal::NumberParser<T, Fn>(this->GetJsonDocument(), std::forward<Fn>(fn)).SubParse();
  }

  /*!
   * \brief Parses the following string value and fails if no string comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on strings
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseString(Fn&& fn) -> internal::CallableReturnsNoResult<Fn, vac::container::CStringView, R> {
    static_assert(internal::ReturnsVoid<Fn, vac::container::CStringView>::value, "Must return void");
    return ParseString([&fn](vac::container::CStringView s) {
      std::forward<Fn>(fn)(s);
      return Result<void>{};
    });
  }

  /*!
   * \brief Parses the following string value and fails if no string comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on strings
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseString(Fn&& fn) -> internal::CallableReturnsResult<Fn, vac::container::CStringView, R> {
    static_assert(internal::ReturnsResultVoid<Fn, vac::container::CStringView>::value, "Must return Result<void>");
    return internal::StringParser<Fn>(this->GetJsonDocument(), std::forward<Fn>(fn)).SubParse();
  }

  /*!
   * \brief Parses the following number values and fails if no array of numbers comes next.
   * \tparam T The number type to parse
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on index + numbers
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename T, typename Fn>
  auto ParseNumberArray(Fn&& fn) -> internal::ArrayCallableReturnsNoResult<Fn, T, R> {
    static_assert(internal::ArrayReturnsVoid<Fn, T>::value, "Must return void");
    return ParseNumberArray<T>([&fn](std::size_t n, T number) {
      std::forward<Fn>(fn)(n, number);
      return Result<void>{};
    });
  }

  /*!
   * \brief Parses the following number values and fails if no array of numbers comes next.
   * \tparam T The number type to parse
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on index + numbers
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename T, typename Fn>
  auto ParseNumberArray(Fn&& fn) -> internal::ArrayCallableReturnsResult<Fn, T, R> {
    static_assert(internal::ArrayReturnsResultVoid<Fn, T>::value, "Must return Result<void>");
    return ParseArray([this, &fn](std::size_t n) {
      return this->ParseNumber<T>([this, &fn, n](T number) { return std::forward<Fn>(fn)(n, number); }).Drop();
    });
  }

  /*!
   * \brief Parses the following string values and fails if no array of strings comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on index + strings
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseStringArray(Fn&& fn) -> internal::ArrayCallableReturnsNoResult<Fn, vac::container::CStringView, R> {
    static_assert(internal::ArrayReturnsVoid<Fn, vac::container::CStringView>::value, "Must return void");
    return ParseStringArray([&fn](std::size_t n, vac::container::CStringView s) {
      std::forward<Fn>(fn)(n, s);
      return Result<void>{};
    });
  }

  /*!
   * \brief Parses the following string values and fails if no array of strings comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on index + strings
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseStringArray(Fn&& fn) -> internal::ArrayCallableReturnsResult<Fn, vac::container::CStringView, R> {
    static_assert(internal::ArrayReturnsResultVoid<Fn, vac::container::CStringView>::value, "Must return Result<void>");
    return ParseArray([this, &fn](std::size_t n) {
      return this->ParseString([this, &fn, n](vac::container::CStringView s) { return std::forward<Fn>(fn)(n, s); })
          .Drop();
    });
  }

  /*!
   * \brief Parses the following values and fails if no array comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on index
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseArray(Fn&& fn) -> internal::CallableReturnsNoResult<Fn, std::size_t, R> {
    static_assert(internal::ReturnsVoid<Fn, std::size_t>::value, "Must return void");
    return ParseArray([&fn](std::size_t n) {
      std::forward<Fn>(fn)(n);
      return Result<void>{};
    });
  }

  /*!
   * \brief Parses the following values and fails if no array comes next.
   * \tparam Fn The function type for the callback
   * \param fn The function to execute on index
   * \return The error of the callback on failure or kRunning on success
   */
  template <typename Fn>
  auto ParseArray(Fn&& fn) -> internal::CallableReturnsResult<Fn, std::size_t, R> {
    static_assert(internal::ReturnsResultVoid<Fn, std::size_t>::value, "Must return Result<void>");
    return internal::ArrayParser<Fn>(this->GetJsonDocument(), std::forward<Fn>(fn)).SubParse();
  }
};
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_STRICT_PARSER_H_
