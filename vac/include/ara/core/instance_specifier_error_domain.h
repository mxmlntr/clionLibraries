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
/**        \file  ara/core/instance_specifier_error_domain.h
 *        \brief  Error domain for errors originating from InstanceSpecifier class.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_INSTANCE_SPECIFIER_ERROR_DOMAIN_H_
#define LIB_VAC_INCLUDE_ARA_CORE_INSTANCE_SPECIFIER_ERROR_DOMAIN_H_

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
 * \brief Specifies the types of errors that can occur upon calling InstanceSpecifier class.
 * \vprivate
 */
enum class InstanceSpecifierErrc : ErrorDomain::CodeType {
  /*!
   *  \brief The invalid path and/or short name for meta model identifier.
   */
  invalid_path_short_name = 101
};

/*!
 * \brief Exception type thrown by InstanceSpecifier class.
 * \vprivate
 */
class InstanceSpecifierException : public Exception {
 public:
  /*!
   * \brief Inheriting constructor.
   */
  using Exception::Exception;
};

/*!
 * \brief Error domain for errors originating from class InstanceSpecifier.
 * \vprivate
 */
class InstanceSpecifierErrorDomain final : public ErrorDomain {
 public:
  /*!
   * \brief The InstanceSpecifier error domain id.
   */
  static constexpr ErrorDomain::IdType kId{0x0206101942492133U};

  /*!
   * \brief Alias for the exception class.
   */
  using Exception = InstanceSpecifierException;

  /*!
   * \brief Default constructor.
   */
  constexpr InstanceSpecifierErrorDomain() noexcept : ErrorDomain(kId) {}

  /*!
   * \brief  Return the "shortname" of this error domain.
   * \return "InstanceSpecifier".
   */
  StringType Name() const noexcept final { return "InstanceSpecifier"; }

  /*!
   * \brief  Translate an error code value into a text message.
   * \param  error_code The error code value.
   * \return The text message, never nullptr.
   *
   */
  StringType Message(ErrorDomain::CodeType error_code) const noexcept final {
    StringType return_msg{"Unknown Error Code"};
    if (error_code == static_cast<std::int32_t>(InstanceSpecifierErrc::invalid_path_short_name)) {
      return_msg = "Invalid path and(or) short name for meta model identifier";
    }
    return return_msg;
  }

  /*!
   * \brief Throw the exception type corresponding to the given ErrorCode.
   * \param error_code the ErrorCode instance
   * @throw ara::core::Exception.
   */
  void ThrowAsException(ErrorCode const& error_code) const noexcept(false) final {
    vac::language::ThrowOrTerminate<Exception>(error_code);
  }
};

namespace internal {

/*!
 * \brief The single global InstanceSpecifierErrorDomain instance.
 * \vprivate
 */
constexpr InstanceSpecifierErrorDomain kInstanceSpecifierErrorDomain;
}  // namespace internal

/*!
 * \brief  Obtain the reference to the single global InstanceSpecifierErrorDomain instance.
 * \return A reference to the InstanceSpecifierErrorDomain instance.
 * \vprivate
 */
constexpr InstanceSpecifierErrorDomain const& GetInstanceSpecifierErrorDomain() noexcept {
  return internal::kInstanceSpecifierErrorDomain;
}

/* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
/*!
 * \brief Create a new ErrorCode for InstanceSpecifierErrorDomain with the given support data type and message.
 * \param code An enumeration value from instance_specifier_errc.
 * \param data A vendor-defined supplementary value.
 * \param message A user-defined context message (can be nullptr).
 * \return The new ErrorCode instance.
 * \vprivate
 */
constexpr ErrorCode MakeErrorCode(InstanceSpecifierErrc code, ErrorDomain::SupportDataType data,
                                  char const* message) noexcept {
  return ErrorCode(static_cast<ErrorDomain::CodeType>(code), GetInstanceSpecifierErrorDomain(), data, message);
}

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_INSTANCE_SPECIFIER_ERROR_DOMAIN_H_
