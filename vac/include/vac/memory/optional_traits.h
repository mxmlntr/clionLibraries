/* VECTOR SL AutosarC++17_10-A2.9.1: MD_VAC_AutosarC++17_10-A2.9.1_headerShouldBeNamedAsATypeItDeclares */
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
/*!        \file  optional_traits.h
 *        \brief  Type-Traits relating to vac::memory::optional.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_OPTIONAL_TRAITS_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_OPTIONAL_TRAITS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <type_traits>
#include "ara/core/optional.h"

namespace vac {
namespace memory {

/*!
 * \brief   Check if type is optional type.
 * \details Implementation if type is not optional.
 */
template <typename C>
struct is_optional : std::false_type {};

/*!
 * \brief   Check if type is optional type.
 * \details Implementation if type is optional.
 */
template <typename T>
struct is_optional<ara::core::Optional<T>> : std::true_type {};

}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_OPTIONAL_TRAITS_H_
