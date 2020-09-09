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
/*!        \file  vac/container/string_view.h
 *        \brief  Aliases for SWS core type ara::core::StringView.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_STRING_VIEW_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_STRING_VIEW_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "ara/core/string_view.h"

namespace vac {
namespace container {

/*!
 * \brief Alias for ara::core::basic_string_view.
 */
template <typename T>
using basic_string_view = ara::core::basic_string_view<T>;
/*!
 * \brief Alias for ara::core::StringView.
 */
using string_view = ara::core::StringView;
/*!
 * \brief Alias for basic_string_view<wchar_t>.
 */
using wstring_view = ara::core::basic_string_view<wchar_t>;
/*!
 * \brief Alias for basic_string_view<char16_t>.
 */
using u16string_view = ara::core::basic_string_view<char16_t>;
/*!
 * \brief Alias for basic_string_view<char32_t>.
 */
using u32string_view = ara::core::basic_string_view<char32_t>;
/*!
 * \brief Alias for ara::core::basic_string_view::npos.
 */
static constexpr std::size_t npos{ara::core::StringView::npos};

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_STRING_VIEW_H_
