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
/*!        \file  cpp17_backport.h
 *        \brief  Contains useful backports of STL features from C++17 to C++11.
 *
 *      \details  The C++17 backported features include also the backported C++14 ones.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_CPP17_BACKPORT_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_CPP17_BACKPORT_H_

#include <utility>
#include "ara/core/utility.h"
#include "vac/language/cpp14_backport.h"

namespace vac {
namespace language {

namespace detail {

/* VECTOR Disable AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_compileTimeResolution */

using std::swap;

/*!
 * \brief Helper template in namespace including swap (aka using swap;) to test if a call to swap is well formed.
 */
struct swap_test {
  /*!
   * \brief Overload for swappable resolution.
   */
  template <typename T, typename = decltype(swap(std::declval<T&>(), std::declval<T&>()))>
  static std::true_type can_swap(int);

  /*!
   * \brief Overload for non-swappable resolution.
   */
  template <typename>
  static std::false_type can_swap(...);
};

/*!
 * \brief Helper template to test if a call to swap is well formed.
 */
template <typename T>
struct swappable_impl : public swap_test {
  /*!
   * \brief Type containing the deduction result.
   */
  typedef decltype(can_swap<T>(0)) type;
};

/*!
 * \brief Test implementation for swappable.
 * \trace CREQ-158610
 */
template <typename T, bool = (std::is_const<typename std::remove_reference<T>::type>::value) ||
                             (std::is_function<typename std::remove_reference<T>::type>::value)>
struct is_swappable;

/*!
 * \brief Specialization if the tested type is const.
 */
template <typename T>
struct is_swappable<T, true> : public std::false_type {};

/*!
 * \brief Specialization if the tested type is non-const.
 */
template <typename T>
struct is_swappable<T, false> : swappable_impl<T>::type {};

/*!
 * \brief Implementation of nothrow swappable.
 */
template <typename T, bool = is_swappable<T>::value>
struct is_nothrow_swappable;

/*!
 * \brief Specialization for swappable types.
 */
template <typename T>
struct is_nothrow_swappable<T, true>
    : std::integral_constant<bool, noexcept(swap(std::declval<T&>(), std::declval<T&>()))> {};

/*!
 * \brief Specialization for non-swappable types.
 */
template <typename T>
struct is_nothrow_swappable<T, false> : std::false_type {};
}  // namespace detail

/*!
 * \brief Is true if swap<T> is possible.
 */
template <typename T>
struct is_swappable : detail::is_swappable<T> {};

/*!
 * \brief Is true if swap<T> is noexcept.
 */
template <typename T>
struct is_nothrow_swappable : detail::is_nothrow_swappable<T> {};

/*!
 * \brief Conjunction.
 */
template <class...>
struct conjunction : std::true_type {};
/*!
 * \brief Conjunction.
 */
template <class B1>
struct conjunction<B1> : B1 {};
/*!
 * \brief Conjunction.
 */
template <class B1, class... Bn>
struct conjunction<B1, Bn...> : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

/*!
 * \brief  Forms lvalue reference to const type of t.
 * \tparam T The value type.
 * \param  t The non-const value.
 * \return The const-reference form of the value.
 * \trace  CREQ-171850
 */
template <typename T>
constexpr add_const_t<T>& as_const(T& t) noexcept {
  return t;
}
/*!
 * \brief  Const rvalue reference overload is deleted to disallow rvalue arguments.
 * \tparam T Value type.
 * \trace  CREQ-171850
 */
template <typename T>
void as_const(const T&&) = delete;

/* VECTOR Enable AutosarC++17_10-A12.4.1 */
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_CPP17_BACKPORT_H_
