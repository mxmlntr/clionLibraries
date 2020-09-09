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
/*!        \file  compile_exceptions.h
 *        \brief  Compile time declarations related to exception handling.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_COMPILE_EXCEPTIONS_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_COMPILE_EXCEPTIONS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

namespace vac {
namespace language {
namespace internal {

/* VECTOR NL AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards */
#ifdef __EXCEPTIONS
/*!
 * \brief Flag for compiling with exceptions (always true).
 */
static constexpr bool kCompileWithExceptions{true};
#else
/*!
 * \brief Flag for compiling with exceptions (always false).
 */
static constexpr bool kCompileWithExceptions{false};
#endif

}  // namespace internal

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_COMPILE_EXCEPTIONS_H_
