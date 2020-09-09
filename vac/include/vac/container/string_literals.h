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
/*!        \file  string_literals.h
 *        \brief  Provides string literals to create string views.
 *
 *      \details  string literals create CStringView at compile time.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_STRING_LITERALS_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_STRING_LITERALS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include "vac/container/c_string_view.h"

namespace vac {
namespace container {
inline namespace literals {
/*!
 * \brief  Creates a CStringView from a string-literal.
 * \param  s The string literal from which to create the view.
 * \param  size The size of the pointer.
 * \return A null-terminated string view.
 */
constexpr CStringView operator"" _sv(char const* s, std::size_t size) noexcept {
  return CStringView::FromLiteral(s, size);
}

/*!
 * \brief  Creates a CStringView from a wide string-literal.
 * \param  s The wide string literal from which to create the view.
 * \param  size The size of the pointer.
 * \return A null-terminated string view.
 */
constexpr WideCStringView operator"" _sv(wchar_t const* s, std::size_t size) noexcept {
  return WideCStringView::FromLiteral(s, size);
}

/*!
 * \brief  Creates a CStringView from a u16 string-literal.
 * \param  s The u16 string literal from which to create the view.
 * \param  size The size of the pointer.
 * \return A null-terminated string view.
 */
constexpr U16CStringView operator"" _sv(char16_t const* s, std::size_t size) noexcept {
  return U16CStringView::FromLiteral(s, size);
}

/*!
 * \brief  Creates a CStringView from a u32 string-literal.
 * \param  s The u32 string literal from which to create the view.
 * \param  size The size of the pointer.
 * \return A null-terminated string view.
 */
constexpr U32CStringView operator"" _sv(char32_t const* s, std::size_t size) noexcept {
  return U32CStringView::FromLiteral(s, size);
}
}  // namespace literals
}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_STRING_LITERALS_H_
