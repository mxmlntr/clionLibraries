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
/*!        \file  ara/core/error_code.h
 *        \brief  SWS core type ara::core::ErrorCode.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_ERROR_CODE_H_
#define LIB_VAC_INCLUDE_ARA_CORE_ERROR_CODE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "ara/core/error_domain.h"
#include "ara/core/string_view.h"
#include "vac/language/reference_wrapper.h"

namespace ara {
namespace core {

/*!
 * \brief   Encapsulation of an error code.
 * \details An ErrorCode contains a raw error code value and an error domain.
 *          The raw error code value is specific to this error domain.
 * \trace   CREQ-166418
 * \vpublic
 */
class ErrorCode final {
  /*!
   * \brief Type of the numerical value.
   */
  using CodeType = typename ErrorDomain::CodeType;

  /*!
   * \brief Type for (vendor specific) support data.
   */
  using SupportDataType = typename ErrorDomain::SupportDataType;

 public:
  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedefs */
  /* VECTOR Next Construct VectorC++-V12-1-4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit */
  /*!
   * \brief   Construct a new ErrorCode instance with parameters.
   * \details This constructor does not participate in overload resolution unless \a EnumT is an enum type.
   *          To execute this constructor an overload of MakeErrorCode(EnumT, SupportDataType, char const*) must be
   *          available via ADL.
   * \tparam  EnumT An enum type that contains error code values.
   * \param   e A domain-specific error code value.
   * \param   data Optional vendor-specific supplementary error context data.
   * \param   user_message An optional user-defined custom static message text (null-terminated).
   * \vpublic
   */
  template <typename EnumT, typename = typename std::enable_if<std::is_enum<EnumT>::value>::type>
  constexpr ErrorCode(EnumT e, SupportDataType data = {},
                      char const* user_message = nullptr) noexcept  // NOLINT(runtime/explicit)
      /*  Call MakeErrorCode() unqualified, so the correct overload is found via ADL. */
      : ErrorCode(MakeErrorCode(e, data, user_message)) {}

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief   Construct a new ErrorCode instance with parameters.
   * \details This constructor does not participate in overload resolution unless \a EnumT is an enum type.
   * \tparam  EnumT An enum type that contains error code values.
   * \param   e A domain-specific error code value.
   * \param   user_message A user-defined custom static message text (null-terminated).
   * \param   data Optional vendor-specific supplementary error context data.
   * \vpublic
   */
  template <typename EnumT, typename = typename std::enable_if<std::is_enum<EnumT>::value>::type>
  constexpr ErrorCode(EnumT e, char const* user_message, SupportDataType data = {}) noexcept
      /*  Call MakeError() unqualified, so the correct overload is found via ADL. */
      : ErrorCode(e, data, user_message) {}

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief   Construct a new ErrorCode instance with parameters.
   * \param   value Domain-specific error code value.
   * \param   domain The ErrorDomain associated with \a value.
   * \param   user_message User-defined custom static message text (null-terminated).
   * \param   data Optional vendor-specific supplementary error context data.
   * \warning The error code stores a reference to the ErrorDomain and not a copy if it. If the lifetime of the
   *          ErrorDomain ends before the ErrorCode, the ErrorCode will hold a dangling reference.
   * \vpublic
   */
  constexpr ErrorCode(CodeType value, ErrorDomain const& domain, SupportDataType data = {},
                      char const* user_message = nullptr) noexcept
      : value_(value), support_data_(data), domain_(domain), user_message_(user_message) {}

  /*!
   * \brief Move constructor.
   * \vpublic
   */
  constexpr ErrorCode(ErrorCode&&) noexcept = default;

  /*!
   * \brief Copy constructor.
   * \vpublic
   */
  constexpr ErrorCode(ErrorCode const&) noexcept = default;

  /*! \brief Move assignment. */
  ErrorCode& operator=(ErrorCode&&) & noexcept = default;

  /*! \brief Copy assignment. */
  ErrorCode& operator=(ErrorCode const&) & noexcept = default;

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*!
   * \brief Destructor.
   * \vpublic
   */
  ~ErrorCode() noexcept = default;

  /*!
   * \brief  Return the raw error code value.
   * \return The raw error code value.
   * \vpublic
   */
  constexpr CodeType Value() const noexcept { return value_; }

  /*!
   * \brief   Return the supplementary error context data.
   * \details The content of the returned value is implementation-defined.
   * \return  The supplementary error context data.
   * \vpublic
   */
  constexpr SupportDataType SupportData() const noexcept { return support_data_; }

  /*!
   * \brief  Return the domain with which this ErrorCode is associated.
   * \return The ErrorDomain.
   * \vpublic
   */
  constexpr ErrorDomain const& Domain() const noexcept { return domain_.get(); }

  /* VECTOR Next Construct AutosarC++17_10-A15.5.3: MD_VAC_A15.4.3_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A15.4.4: MD_VAC_A15.4.4_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /*!
   * \brief  Return the user-specified message text.
   * \return The user-specified message text, or an empty string if none was given.
   * \vpublic
   */
  constexpr StringView UserMessage() const noexcept { return StringView{user_message_}; }

  /* VECTOR Next Construct AutosarC++17_10-A15.5.3: MD_VAC_A15.4.3_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A15.4.4: MD_VAC_A15.4.4_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /*!
   * \brief  Return a textual representation of this ErrorCode.
   * \return A null-terminated error message text, never nullptr.
   * \vpublic
   */
  StringView Message() const noexcept { return StringView{Domain().Message(Value())}; }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.5: MD_VAC_A15.4.5_documentCheckedException */
  /*!
   * \brief   Throws the error code as exception.
   * \details The exception type depends on the domain and the numerical value of this error code.
   *          If ARA_NO_EXCEPTIONS is defined this function call will terminate.
   * \vprivate
   */
  void ThrowAsException() const { Domain().ThrowAsException(*this); }

  /*!
   * \brief  Sets the support data field for this error code.
   * \param  support_data The support data to set.
   * \return A reference to *this.
   * \vprivate
   */
  ErrorCode& SetSupportData(SupportDataType support_data) & {
    this->support_data_ = support_data;
    return *this;
  }

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Sets the user message field.
   * \param  user_message The user message to set.
   * \return A reference to *this.
   * \vprivate
   */
  ErrorCode& SetUserMessage(char const* user_message) & {
    this->user_message_ = user_message;
    return *this;
  }

  /*!
   * \brief  Copies the ErrorCode but changes the support data field.
   * \param  support_data The support data to set.
   * \return A changed copy of *this.
   * \vprivate
   */
  ErrorCode WithSupportData(SupportDataType support_data) const {
    ErrorCode copy{*this};
    copy.support_data_ = support_data;
    return copy;
  }

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Copies the ErrorCode but changes the user message field.
   * \param  user_message The user message to set.
   * \return A changed copy of *this.
   * \vprivate
   */
  ErrorCode WithUserMessage(char const* user_message) const {
    ErrorCode copy{*this};
    copy.user_message_ = user_message;
    return copy;
  }

 private:
  /*! \brief Numerical error code value. */
  CodeType value_;
  /*! \brief Support data (vendor specific). */
  SupportDataType support_data_;

  /*! \brief Domain defining the context of this error code. */
  vac::language::reference_wrapper<ErrorDomain const> domain_;

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*! \brief User message specific for this error message. */
  char const* user_message_;
};

/*!
 * \brief   Global operator== for ErrorCode.
 * \details Two ErrorCode instances compare equal if the results of their Value() and
 *          Domain() functions are equal. The results of SupportData() and UserMessage()
 *          are not considered for equality.
 * \param   lhs The left hand side object of the comparison.
 * \param   rhs The right hand side object of the comparison.
 * \return  True if the two instances compare equal, false otherwise.
 * \vpublic
 */
constexpr inline bool operator==(ErrorCode const& lhs, ErrorCode const& rhs) {
  return (lhs.Domain() == rhs.Domain()) && (lhs.Value() == rhs.Value());
}

/*!
 * \brief   Global operator!= for ErrorCode.
 * \details Two ErrorCode instances compare equal if the results of their Value() and
 *          Domain() functions are equal. The results of SupportData() and UserMessage()
 *          are not considered for equality.
 * \param   lhs The left hand side object of the comparison.
 * \param   rhs The right hand side object of the comparison.
 * \return  True if the two instances compare not equal, false otherwise.
 * \vpublic
 */
constexpr inline bool operator!=(ErrorCode const& lhs, ErrorCode const& rhs) { return !operator==(lhs, rhs); }

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_ERROR_CODE_H_
