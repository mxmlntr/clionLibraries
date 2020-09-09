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
/*!        \file  ara/core/error_domain.h
 *        \brief  SWS core type ara::core::ErrorDomain.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_ERROR_DOMAIN_H_
#define LIB_VAC_INCLUDE_ARA_CORE_ERROR_DOMAIN_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdint>

namespace ara {
namespace core {

// Forward declaration.
class ErrorCode;

/*!
 * \brief   Encapsulation of an error domain.
 * \details An error domain is the controlling entity for ErrorCode's error code values, and defines the mapping of
 *          such error code values to textual representations.
 *          This class is a literal type, and subclasses are strongly advised to be literal types as well.
 * \trace   CREQ-166419
 * \vpublic
 */
class ErrorDomain { /* VECTOR Same Line AutosarC++17_10-A12.4.2: MD_VAC_A12.4.2_classNotFinal */
 public:
  /*!
   * \brief Alias type for a unique ErrorDomain identifier type.
   */
  using IdType = std::uint64_t;

  /*!
   * \brief Alias type for a domain-specific error code.
   */
  using CodeType = std::int32_t;

  /*!
   * \brief Alias type for vendor- and context-specific supplementary data.
   */
  using SupportDataType = std::int32_t;

  /*!
   * \brief Alias type for the used string.
   * \vprivate
   */
  using StringType = char const*;

  /*!
   * \brief Default copy constructor deleted.
   * \vpublic
   */
  ErrorDomain(ErrorDomain const&) = delete;

  /*!
   * \brief Default move constructor deleted.
   * \vpublic
   */
  ErrorDomain(ErrorDomain&&) = delete; /* VECTOR Same Line AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_deletedFunction */

  /*! \brief Default copy assignment operator deleted. */
  ErrorDomain& operator=(ErrorDomain const&) & = delete;

  /*! \brief Default move assignment operator deleted. */
  /* VECTOR Next Line AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_deletedFunction */
  ErrorDomain& operator=(ErrorDomain&&) & = delete;

  /*!
   * \brief   Return the name of this error domain.
   * \returns The name as a null-terminated string, never nullptr.
   * \vpublic
   */
  virtual StringType Name() const noexcept = 0;

  /*!
   * \brief   Return a textual representation of the given error code.
   * \details The return value is undefined if the \a errorCode did not originate from this error domain.
   * \param   error_code The domain-specific error code.
   * \returns The text as a null-terminated string, never nullptr.
   * \vpublic
   */
  virtual StringType Message(CodeType error_code) const noexcept = 0;

  /*!
   * \brief  Throws the given errorCode as Exception.
   * \param  error_code Error code to be thrown.
   * \remark If the code is compiled without exceptions, this function call will terminate.
   * \trace  CREQ-166428
   * \vpublic
   */
  [[noreturn]] virtual void ThrowAsException(ErrorCode const& error_code) const noexcept(false) = 0;

  /*!
   * \brief   Return the unique domain identifier.
   * \returns The identifier.
   * \trace   CREQ-166429
   * \vpublic
   */
  constexpr IdType Id() const noexcept { return id_; }

  /*!
   * \brief   Compare for equality with another ErrorDomain instance.
   * \details Two ErrorDomain instances compare equal when their identifiers (returned by Id()) are equal.
   * \param   other The other instance.
   * \returns True if \a other is equal to *this, false otherwise.
   * \vpublic
   */
  constexpr bool operator==(ErrorDomain const& other) const noexcept { return id_ == other.id_; }

  /*!
   * \brief   Compare for non-equality with another ErrorDomain instance.
   * \param   other The other instance.
   * \returns True if \a other is not equal to *this, false otherwise.
   * \vpublic
   */
  constexpr bool operator!=(ErrorDomain const& other) const noexcept { return !operator==(other); }

 protected:
  /*!
   * \brief   Construct a new instance with the given identifier.
   * \details Identifiers are expected to be system-wide unique.
   * \param   id The unique identifier.
   * \vpublic
   */
  constexpr explicit ErrorDomain(IdType id) noexcept : id_(id) {}

  /*!
   * \brief   Destructor.
   * \details This dtor is non-virtual (and trivial) so that this class can be a literal type. While this class has
   *          virtual functions, no polymorphic destruction is needed.
   * \vpublic
   */
  ~ErrorDomain() = default;

 private:
  /*! \brief Id of the error domain. */
  IdType const id_;
};

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_ERROR_DOMAIN_H_
