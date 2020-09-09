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
/*!        \file  common_error_domain.h
 *        \brief  Error Domain for common errors.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_COMMON_ERROR_DOMAIN_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_COMMON_ERROR_DOMAIN_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <array>
#include <limits>

#include "ara/core/exception.h"
#include "vac/language/error_code.h"
#include "vac/language/error_domain.h"
#include "vac/language/throw_or_terminate.h"

namespace vac {
namespace language {

/*!
 * \brief Common error code enum.
 */
enum class CommonErrc : ErrorDomain::CodeType {
  not_implemented = 1,      /*!< Function is not implemented. */
  uninitialized_result = 2, /*!< No specific value or error emplaced. */
  empty_optional = 3,       /*!< Error for conversion from Optional to Result if optional was empty. */
};

/*!
 * \brief Exception type for CommonErrorDomain.
 */
class CommonException final : public ara::core::Exception {
 public:
  /*!
   * \brief Inherit constructor.
   */
  using Exception::Exception;
};

/*!
 * \brief Error domain for Common system errors.
 * \trace CREQ-180529
 */
class CommonErrorDomain final : public ErrorDomain {
  /*!
   * \brief ID of the domain.
   */
  static constexpr ErrorDomain::IdType kId{0x123457};

 public:
  /*!
   * \brief Error code enum of the domain.
   */
  using Errc = CommonErrc;
  /*!
   * \brief Exception type of the domain.
   */
  using Exception = CommonException;

  /*!
   * \brief Constructor. Initializes the domain id with CommonErrorDomain::kId.
   */
  constexpr CommonErrorDomain() noexcept : ErrorDomain(kId) {}

  /*!
   * \copybrief ErrorDomain::Name()
   * \return    "Common".
   */
  char const* Name() const noexcept final { return "Common"; }

  /*!
   * \copydoc ErrorDomain::Message()
   */
  char const* Message(ErrorDomain::CodeType error_code) const noexcept final {
    constexpr static std::array<char const*, 4> kMessages{{"Unknown Error Code", "Function is not implemented",
                                                           "Result was not initialized with a specific value or error",
                                                           "Optional converted to a Result was empty"}};
    static_assert(std::numeric_limits<std::size_t>::max() >=
                      static_cast<std::size_t>(std::numeric_limits<ErrorDomain::CodeType>::max()),
                  "ErrorDomain::CodeType must not exceed std::size_t.");

    std::size_t index{0};
    if (error_code > 0) {
      std::size_t const code{static_cast<std::size_t>(error_code)};
      if (code < kMessages.size()) {
        index = code;
      }
    }
    return kMessages[index];
  }

  /*!
   * \brief  Throws the given errorCode as Exception.
   * \param  error_code Error code to be thrown.
   * \throw  CommonException for all error codes.
   * \remark If the is compiled without exceptions, this function call will terminate.
   */
  [[noreturn]] void ThrowAsException(ErrorCode const& error_code) const noexcept(false) final {
    ThrowOrTerminate<Exception>(error_code);
  }
};

namespace internal {
/*!
 * \brief Global CommonErrorDomain instance.
 * \trace CREQ-180529
 */
constexpr CommonErrorDomain kCommonErrorDomain;
}  // namespace internal

/*!
 * \brief  Returns a reference to the global CommonErrorDomain instance.
 * \return CommonErrorDomain instance.
 */
inline constexpr ErrorDomain const& GetCommonDomain() noexcept { return internal::kCommonErrorDomain; }

/* VECTOR Next Construct AutosarC++17_10-A2.11.4: MD_VAC_A2.11.4_nameOfObjectOrFunctionReused */
/*!
 * \brief  Creates an ErrorCode from the CommonErrorDomain.
 * \param  code Error code enum value.
 * \param  data Support data.
 * \param  message User message.
 * \return The created error code.
 * \remark The lifetime of the message should exceed the lifetime of the ErrorCode. Otherwise ErrorCode::UserMessage may
 *         return dangling reference.
 */
inline constexpr ErrorCode MakeErrorCode(CommonErrorDomain::Errc code, ErrorDomain::SupportDataType data,
                                         char const* message) noexcept {
  return ErrorCode(static_cast<ErrorDomain::CodeType>(code), GetCommonDomain(), data, message);
}
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_COMMON_ERROR_DOMAIN_H_
