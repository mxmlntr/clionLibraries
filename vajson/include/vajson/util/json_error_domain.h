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
/**        \file
 *        \brief  Contains error handling related types
 *      \details  Contains Result alias, error domain & error codes
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_UTIL_JSON_ERROR_DOMAIN_H_
#define LIB_VAJSON_INCLUDE_VAJSON_UTIL_JSON_ERROR_DOMAIN_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "ara/core/optional.h"
#include "ara/core/result.h"
#include "vac/language/error_code.h"
#include "vac/language/error_domain.h"
#include "vac/language/exception.h"
#include "vac/language/throw_or_terminate.h"

namespace vajson {

/*!
 * \brief Null-terminated C-string
 *
 * \vprivate
 */
using CStr = char const*;

/*!
 * \brief An alias of ara::core::Result for vajson
 * \tparam T The type of value
 *
 * \vprivate
 */
template <typename T>
using Result = ara::core::Result<T>;

/*!
 * \brief An alias of ara::core::Optional for vajson
 * \tparam T The type of value
 *
 * \vprivate
 */
template <typename T>
using Optional = ara::core::Optional<T>;

/*!
 * \brief Alias for ErrorDomain.
 *
 * \vprivate
 */
using ErrorDomain = vac::language::ErrorDomain;

/*!
 * \brief Alias for ErrorCode.
 *
 * \vprivate
 */
using ErrorCode = vac::language::ErrorCode;

/*!
 * \brief Contains all error codes thrown by vajson
 *
 * \vpublic
 */
enum class JsonErrc : ErrorDomain::CodeType {
  kNotInitialized,
  kUnexpectedEOF,
  kInvalidState,
  kKeyTooLong,
  kStringTooLong,
  kTreeDepthError,
  kUnexpectedOpeningBrackets,
  kUnexpectedClosingBrackets,
  kExpectedClosingBrackets,
  kUnexpectedOpeningBraces,
  kUnexpectedClosingBraces,
  kExpectedClosingBraces,
  kExpectedKey,
  kExpectedValue,

  kInvalidNullLiteral,   // unused
  kInvalidTrueLiteral,   // unused
  kInvalidFalseLiteral,  // unused
  kInvalidNumber,
  kInvalidString,
  kInvalidType,

  kNotInObject,
  kNotInArray,
  kCannotExitObject,  // unused
  kCannotExitArray,   // unused
  kUnexpectedOnTopLevel,
  kUnicodeEscape,
  kUserValidationFailed,
  user_validation_failed,  // Legacy Name
};

/*!
 * \brief Exception type for all JSON related errors
 *
 * \vpublic
 */
class JsonException final : public vac::language::Exception {
 public:
  /*! \brief Inherit the constructor */
  using Exception::Exception;
};

/*!
 * \brief The error domain for all JSON related errors
 *
 * \vprivate
 */
class JsonErrorDomain final : public ErrorDomain {
  /*!
   * \brief The unique domain id
   */
  constexpr static IdType kId{0x424242};

 public:
  /*!
   * \brief Implements the Errc interface-type
   */
  using Errc = JsonErrc;
  /*!
   * \brief Implements the Exception interface-type
   */
  using Exception = JsonException;

  /*!
   * \brief Default constructor
   */
  constexpr JsonErrorDomain() noexcept : ErrorDomain(kId) {}

  /*!
   * \brief Gets the name of the Error domain
   * \returns the name of the Error domain
   */
  CStr Name() const noexcept final;

  /*!
   * \brief Converts an error code into a message
   * \param error_code The error code to transform
   * \return The transformed message
   */
  CStr Message(CodeType error_code) const noexcept final;

  /*!
   * \brief Converts the error into an exception
   * \param error_code The error code to throw
   * \throws JsonException with the passed error code
   */
  [[noreturn]] void ThrowAsException(ErrorCode const& error_code) const noexcept(false) final;
};

namespace error {
namespace internal {
/*!
 * \brief The Json error domain singleton
 */
constexpr JsonErrorDomain g_JsonErrorDomain;
}  // namespace internal
}  // namespace error

/*!
 * \brief Gets the error domain singleton
 * \returns the error domain singleton
 *
 * \vprivate
 */
inline constexpr ErrorDomain const& GetJsonDomain() { return error::internal::g_JsonErrorDomain; }

/*!
 * \brief Creates an error code from JsonErrorDomain
 * \param code The specific error code
 * \param data Vendor defined support data
 * \param message An optional message for this error
 * \returns The constructed error code
 *
 * \vprivate
 */
inline constexpr ErrorCode MakeErrorCode(JsonErrorDomain::Errc code, ErrorDomain::SupportDataType data, CStr message) {
  return {static_cast<ErrorDomain::CodeType>(code), GetJsonDomain(), data, message};
}

/*!
 * \brief Creates an error code from JsonErrorDomain
 * \param code The specific error code
 * \param message An optional message for this error
 * \returns The constructed error code
 *
 * \vprivate
 */
inline constexpr ErrorCode MakeErrorCode(JsonErrorDomain::Errc code, CStr message) {
  return {static_cast<ErrorDomain::CodeType>(code), GetJsonDomain(), 0, message};
}

/*!
 * \brief Creates a Result from a boolean value
 * \param value The value to convert
 * \param code The error code to use
 * \param message The optional message to use
 * \param data The optional support data
 * \return either a successful result or the specified error
 *
 * \vprivate
 */
inline auto MakeResult(bool value, JsonErrorDomain::Errc code, CStr message = "", ErrorDomain::SupportDataType data = 0)
    -> Result<void> {
  Result<void> result{};
  if (not value) {
    result.EmplaceError(MakeErrorCode(code, data, message));
  }
  return result;
}

/*!
 * \brief Creates a Result from a boolean value
 * \tparam Fn The function type creating an Error code
 * \param value The value to convert
 * \param fn The function creating an Error code
 * \return either a successful result or the specified error
 *
 * \vprivate
 */
template <typename Fn>
auto MakeResult(bool value, Fn&& fn) -> Result<void> {
  Result<void> result{};
  if (not value) {
    result.EmplaceError(fn());
  }
  return result;
}

/*!
 * \brief Creates a Result from an optional value
 * \param value The value to convert
 * \param code The error code to use
 * \param message The optional message to use
 * \param data The optional support data
 * \return either a successful result or the specified error
 *
 * \vprivate
 */
template <typename T>
auto MakeResult(Optional<T> value, JsonErrorDomain::Errc code, CStr message = "", ErrorDomain::SupportDataType data = 0)
    -> Result<T> {
  using R = Result<T>;
  return value.has_value() ? R{std::move(*value)} : R::FromError(MakeErrorCode(code, data, message));
}

}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_UTIL_JSON_ERROR_DOMAIN_H_
