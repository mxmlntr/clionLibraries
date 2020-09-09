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
/*!        \file  vac/memory/optional.h
 *        \brief  (Partial) Backport of std::optional from C++17 as vac::memory::optional.
 *
 *      \details  optional wrapper returns a reference to the contained value if the contained value exists.
 *                Otherwise, it throws an exception.
 *                optional wrapper defines operators to access the contained value. Return undefined behavior
 *                if the object does not contain any value.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_OPTIONAL_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_OPTIONAL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>
#include "ara/core/optional.h"

namespace vac {
namespace memory {

/*!
 * \brief Class to manage a static memory location in which an object of given type can be constructed.
 *        (Partial) Backport from C++17.
 */
template <typename T>
using optional = ara::core::Optional<T>;

/* VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride */
/*!
 * \brief  Creates an optional object from value.
 * \param  value The value to construct optional object with.
 * \return The constructed optional object.
 */
template <typename T>
auto make_optional(T&& value) noexcept(false) -> optional<std::decay_t<T>> {
  return optional<std::decay_t<T>>{std::forward<T>(value)};
}

/* VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride */
/* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
/*!
 * \brief  Creates an optional object constructed in-place from args.
 * \param  args Arguments to be passed to the constructor of T.
 * \return The constructed optional object.
 */
template <typename T, typename... Args>
auto make_optional(Args&&... args) noexcept(false) -> optional<T> {
  return optional<T>(ara::core::in_place, std::forward<Args>(args)...);
}

/* VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride */
/* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
/*!
 * \brief  Creates an optional object constructed in-place from args.
 * \param  il Initializer list to be passed to the constructor of T.
 * \param  args Arguments to be passed to the constructor of T.
 * \return The constructed optional object.
 */
template <typename T, typename U, typename... Args>
auto make_optional(std::initializer_list<U> il, Args&&... args) noexcept(false) -> optional<T> {
  return optional<T>(ara::core::in_place, il, std::forward<Args>(args)...);
}

}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_OPTIONAL_H_
