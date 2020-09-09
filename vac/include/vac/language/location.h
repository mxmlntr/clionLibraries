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
/*!        \file  location.h
 *        \brief  LOCATION macro.
 *
 *      \details  The LOCATION macro returns the raw filename from the standard __FILE__ macro without the path.
 *                Using this in a static_assert will break compilation in GCC5 and GCC6, even though it is done in
 *                compile time.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_LOCATION_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_LOCATION_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <cstddef>

namespace vac {
namespace language {
namespace internal {
/*!
 * \brief  Extracts a substring from a zero terminated c-string, which contains all characters after the last slash.
 * \param  str Pointer to the initial character of a zero terminated c-string.
 * \return A pointer to the first character after the last slash.
 */
constexpr char const* SubstrPastLastSlash(char const* str) noexcept {
  char const* substr = str;

  while (*str != 0) {
    // VECTOR Next Line AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmetic
    if (*(str++) == '/') {
      substr = str;
    }
  }

  return substr;
}

}  // namespace internal
}  // namespace language
}  // namespace vac

/*!
 * \brief   LOCATION macro to get the current filename. For details see AMSR-23504.
 * \details This macro should always be evaluated at compile time. Tested with several compilers on godbolt.org.
 */
#define LOCATION                                                                        \
  []() {                                                                                \
    constexpr char const* loc = vac::language::internal::SubstrPastLastSlash(__FILE__); \
    return loc;                                                                         \
  }()

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_LOCATION_H_
