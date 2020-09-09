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
/*!        \file  ara/core/future_error_domain.h
 *        \brief  Error domain for errors originating from classes Future and Promise.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_FUTURE_ERROR_DOMAIN_H_
#define LIB_VAC_INCLUDE_ARA_CORE_FUTURE_ERROR_DOMAIN_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <limits>
#include <utility>
#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"
#include "vac/language/throw_or_terminate.h"

namespace ara {
namespace core {

/*!
 * \brief Specifies the types of internal errors that can occur upon calling Future::get or Future::GetResult.
 * \trace SPEC-7552451
 * \vpublic
 */
enum class future_errc : ErrorDomain::CodeType {
  broken_promise = 101,            /*! \brief The asynchronous task abandoned its shared state */
  future_already_retrieved = 102,  /*! \brief The contents of the shared state were already accessed */
  promise_already_satisfied = 103, /*! \brief Attempt to store a value into the shared state twice */
  no_state = 104,                  /*! \brief Attempt to access Promise or Future without an associated state */
};

/*!
 * \brief Exception type thrown by Future and Promise classes.
 * \trace SPEC-7552452
 * \trace CREQ-200634
 * \vpublic
 */
class FutureException : public Exception {
 public:
  /*!
   * \brief Inheriting constructor.
   * \trace SPEC-7552453
   * \vpublic
   */
  using Exception::Exception;
};

/*!
 * \brief Error domain for errors originating from classes Future and Promise.
 * \trace SPEC-7552454
 * \trace CREQ-200635
 * \vpublic
 */
class FutureErrorDomain final : public ErrorDomain {
  /*!
   * \brief The Future error domain id
   * \trace SPEC-7552370
   * \trace SPEC-7552372
   */
  constexpr static ErrorDomain::IdType kId{0x8000000000000013U};

 public:
  /*!
   * \brief Alias for the error code value enumeration.
   * \trace SPEC-7552455
   */
  using Errc = future_errc;

  /*!
   * \brief Alias for the exception base class.
   * \trace SPEC-7552456
   */
  using Exception = FutureException;

  /*!
   * \brief Default constructor.
   * \trace SPEC-7552457
   * \trace SPEC-7552372
   * \vpublic
   */
  constexpr FutureErrorDomain() noexcept : ErrorDomain(kId) {}

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
   * \return "Future".
   * \trace  SPEC-7552458
   * \trace  SPEC-7552372
   * \vpublic
   */
  char const* Name() const noexcept final { return "Future"; }

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /* VECTOR Next Construct AutosarC++17_10-A2.11.5: MD_VAC_A2.11.5_nameOfObjectOrFunctionReused */
  /*!
   * \brief  Translates an error code value into a text message.
   * \param  error_code The error code value to translate.
   * \return The text message, never nullptr.
   * \trace  SPEC-7552459
   * \vpublic
   */
  char const* Message(ErrorDomain::CodeType error_code) const noexcept final {
    constexpr static std::array<char const*, 5> kMessages{
        {"Unknown Error Code", "The asynchronous task abandoned its shared state",
         "The contents of the shared state were already accessed",
         "Attempt to store a value into the shared state twice",
         "Attempt to access Promise or Future without an associated state"}};
    static_assert(std::numeric_limits<std::size_t>::max() >=
                      static_cast<std::size_t>(std::numeric_limits<ErrorDomain::CodeType>::max()),
                  "ErrorDomain::CodeType must not exceed std::size_t.");

    std::size_t index{0};
    constexpr static ErrorDomain::CodeType enum_base_value{100};  // Base value that the enum future_errc starts from
    if (error_code > enum_base_value) {
      ErrorDomain::CodeType result{error_code - enum_base_value};
      std::size_t const code{static_cast<std::size_t>(result)};
      if (code < kMessages.size()) {
        index = code;
      }
    }
    return kMessages[index];
  }

  /*!
   * \brief Throw the exception type corresponding to the given ErrorCode.
   * \param error_code The ErrorCode instance.
   * \trace SPEC-7552460
   * \vpublic
   */
  void ThrowAsException(ErrorCode const& error_code) const noexcept(false) final {
    vac::language::ThrowOrTerminate<Exception>(error_code);
  }
};

namespace internal {
/*!
 * \brief The single global FutureErrorDomain instance.
 * \vprivate
 */
constexpr FutureErrorDomain g_futureErrorDomain;
}  // namespace internal

/*!
 * \brief  Obtain the reference to the single global FutureErrorDomain instance.
 * \return Reference to the FutureErrorDomain instance.
 * \trace  SPEC-7552461
 * \vpublic
 */
inline constexpr ErrorDomain const& GetFutureDomain() noexcept { return internal::g_futureErrorDomain; }

/* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
/* VECTOR Next Construct AutosarC++17_10-A2.11.5: MD_VAC_A2.11.5_nameOfObjectOrFunctionReused */
/* VECTOR Next Construct AutosarC++17_10-A2.11.4: MD_VAC_A2.11.4_nameOfObjectOrFunctionReused */
/*!
 * \brief  Create a new ErrorCode for FutureErrorDomain with the given support data type and message.
 * \param  code Enumeration value from future_errc.
 * \param  data Vendor-defined supplementary value.
 * \param  message User-defined context message (can be nullptr).
 * \return The new ErrorCode instance.
 * \trace  SPEC-7552462
 * \vpublic
 */
inline constexpr ErrorCode MakeErrorCode(future_errc code, ErrorDomain::SupportDataType data,
                                         char const* message) noexcept {
  return ErrorCode(static_cast<ErrorDomain::CodeType>(code), GetFutureDomain(), data, message);
}

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_FUTURE_ERROR_DOMAIN_H_
