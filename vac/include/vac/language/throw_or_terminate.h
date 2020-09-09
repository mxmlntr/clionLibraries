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
/*!        \file  throw_or_terminate.h
 *        \brief  ThrowOrTerminate implementation for differentiating between throw and terminate
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_THROW_OR_TERMINATE_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_THROW_OR_TERMINATE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <iostream>
#include <utility>

#include "vac/language/compile_exceptions.h"

namespace vac {
namespace language {
namespace internal {

/*!
 * \brief Calls std::terminate.
 */
[[noreturn]] inline void Terminate() noexcept { std::terminate(); } /* COV_VAC_EXPECT_DEATH */

// VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef
/*!
 * \brief Prints the message on the error console and calls terminate.
 * \param message Termination message.
 */
[[noreturn]] inline void Terminate(char const* message) { /* COV_VAC_EXPECT_DEATH */
  std::cerr << message << std::endl;
  Terminate();
}
/*!
 * \brief   Helper template to control the behavior of ThrowOrTerminate::Execute<ExceptionType>.
 * \details throw_exceptions Execute will throw, otherwise call will terminate.
 * \tparam  throw_exceptions Flag for enabling/disabling exceptions.
 * \trace   CREQ-180467
 */
template <bool throw_exceptions>
struct ThrowOrTerminateT;

/*!
 * \brief Overload when throw_exceptions ==  true.
 */
template <>
struct ThrowOrTerminateT<true> {
  /*!
   * \brief  Throws the given error_code as the given Exception type.
   * \param  args Arguments passed to thse thrown exception.
   * \tparam ExceptionType Exception type to be thrown.
   * \return
   */
  template <class ExceptionType, typename... Args>
  [[noreturn]] static auto Execute(Args&&... args) -> void {
#ifdef __EXCEPTIONS
    /* VECTOR Next Line AutosarC++17_10-M5.2.12: MD_VAC_M5.2.12_arraytoPointerDecay */
    throw ExceptionType(std::forward<Args>(args)...);
#else
    static_assert(false, "This cannot be compiled with -fno-exceptions.");
#endif
  }
};

/*!
 * \brief Helper to cleanly discard parameter packs.
 */
struct ParameterSink {
  template <typename... Args>
  /*!
   * \brief Default contsructor. Discards the parameter pack.
   */
  explicit ParameterSink(Args const&...) {} /* COV_VAC_EXPECT_DEATH */
};

/*!
 * \brief Overload when throw_exceptions == false.
 */
template <>
struct ThrowOrTerminateT<false> {
  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef
  /*!
   * \brief  Calls Terminate() with the given message.
   * \tparam ExceptionType Is ignored.
   * \param  message Error message.
   * \return
   */
  template <class ExceptionType>
  [[noreturn]] static auto Execute(char const* message) noexcept -> void { /* COV_VAC_EXPECT_DEATH */
    Terminate(message);
  }

  /*!
   * \brief  Calls Terminate().
   * \tparam ExceptionType Is ignored.
   * \tparam Args Ignored arguments.
   * \param  args Ignored arguments.
   * \return
   */
  template <class ExceptionType, typename... Args>
  [[noreturn]] static auto Execute(Args&&... args) noexcept -> void { /* COV_VAC_EXPECT_DEATH */
    // Omitting parameter name is not possible since doxygen enforces parameter documentation for parameter pack.
    ParameterSink{args...};
    Terminate();
  }
};
}  // namespace internal

/*!
 * \brief  Throws the specified exception or terminates when excpeption are deactivated.
 * \tparam ExceptionType Exception type.
 * \tparam Args Argument types for exception construction.
 * \param  args Arguments for exception construction.
 */
template <class ExceptionType, typename... Args>
[[noreturn]] void ThrowOrTerminate(Args&&... args) {
  /*!
   * \brief An alias for internal::ThrowOrTerminateT<..>.
   */
  using ThrowOrTerminateType = internal::ThrowOrTerminateT<internal::kCompileWithExceptions>;
  ThrowOrTerminateType::Execute<ExceptionType>(std::forward<Args>(args)...);
}

}  // namespace language
}  // namespace vac

/* COV_JUSTIFICATION_BEGIN
 * \ID COV_VAC_EXPECT_DEATH
 *   \ACCEPT TF
 *   \REASON The function could not be covered due to the fact that the death test is not caught by Bullseye.
 *           The function is covered by test case.
 * COV_JUSTIFICATION_END
 */

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_THROW_OR_TERMINATE_H_
