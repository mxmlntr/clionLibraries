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
/*!        \file  vac/language/result.h
 *        \brief  Alias for SWS core type ara::core::Result.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_RESULT_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_RESULT_H_

/*********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "ara/core/result.h"
#include "vac/language/error_code.h"

namespace vac {
namespace language {

/*!
 * \copydoc ara::core::Result
 */
template <typename T, typename E = ara::core::ErrorCode>
using Result = ara::core::Result<T, E>;

/*!
 * \brief Trait that detects whether a type is a Result<...>.
 * \details SFINAE pattern: General template with typedef type false.
 * \tparam U Arbitrary placeholder for SFINAE pattern.
 */
template <typename U>
struct is_result : std::false_type {};

/*!
 * \brief Trait that detects whether a type is a Result<...>.
 * \details SFINAE pattern: Specialization for Result<U, G>.
 * \tparam U Value of Result.
 * \tparam G Error of Result.
 */
template <typename U, typename G>
struct is_result<Result<U, G>> : std::true_type {};

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_RESULT_H_
