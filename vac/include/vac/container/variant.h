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
/*!        \file  vac/container/variant.h
 *        \brief  Aliases for SWS core type ara::core::Variant.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_VARIANT_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_VARIANT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdint>
#include <utility>
#include "ara/core/variant.h"

namespace vac {
namespace container {

/*!
 * \brief  Variant data type that represents a type-safe union.
 * \tparam Ts Types that may be stored in this variant.
 */
template <typename... Ts>
using variant = ara::core::Variant<Ts...>;

/*!
 * \brief Exception thrown on invalid accesses to the variant.
 */
using bad_variant_access = ara::core::bad_variant_access;

/*!
 * \brief Provides compile-time index access to the variant types.
 */
template <std::size_t I, typename T>
using variant_alternative = ara::core::variant_alternative<I, T>;

/*!
 * \brief Provides compile-time index access to the variant types.
 */
template <std::size_t I, typename T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

/*!
 * \brief Is a marker for an invalid element in variant.
 */
constexpr std::size_t variant_npos = ara::core::variant_npos;

/*!
 * \brief Provides access to the number of alternatives in a possibly cv-qualified variant as a compile-time constant
 *        expression.
 */
template <typename T>
using variant_size = ara::core::variant_size<T>;

/*!
 * \brief Provides access to the number of alternatives in a possibly cv-qualified variant as a compile-time constant
 *        expression.
 */
template <typename T>
constexpr size_t variant_size_v = ara::core::variant_size_v<T>;

/*!
 * \brief  Checks if the contained value is of the passed type.
 * \param  v The variant that shall be tested.
 * \return If the variant contains a value of the passed type.
 */
template <typename T, typename... Types>
bool holds_alternative(variant<Types...> const& v) noexcept {
  return ara::core::holds_alternative<T>(v);
}

/*!
 * \brief  Get variant value from index as pointer.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained or nullptr.
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, variant<Types...>>* get_if(variant<Types...>* v) {
  return ara::core::get_if<N>(v);
}

/*!
 * \brief  Get variant value from index as pointer.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained or nullptr.
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, variant<Types...>> const* get_if(variant<Types...> const* v) {
  return ara::core::get_if<N>(v);
}

/*!
 * \brief  Get variant value from type as pointer.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained or nullptr.
 */
template <typename T, typename... Types>
typename std::add_pointer<T>::type get_if(variant<Types...>* v) {
  return ara::core::get_if<T>(v);
}

/*!
 * \brief  Get variant value from type as pointer.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained or nullptr.
 */
template <typename T, typename... Types>
typename std::add_pointer<T const>::type get_if(variant<Types...> const* v) {
  return ara::core::get_if<T>(v);
}

/*!
 * \brief  Get variant value from index.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained.
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, variant<Types...>>& get(variant<Types...>& v) {
  return ara::core::get<N>(v);
}

/*!
 * \brief  Get variant value from index.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained.
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, variant<Types...>>&& get(variant<Types...>&& v) {
  return ara::core::get<N>(std::move(v));
}

/*!
 * \brief  Get variant value from index.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained.
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, variant<Types...>> const& get(variant<Types...> const& v) {
  return ara::core::get<N>(v);
}

/*!
 * \brief  Get variant value from index.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained.
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, variant<Types...>> const&& get(variant<Types...> const&& v) {
  return ara::core::get<N>(std::move(v));
}

/*!
 * \brief  Get variant value from type.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained.
 */
template <typename T, typename... Types>
T& get(variant<Types...>& v) {
  return ara::core::get<T>(v);
}

/*!
 * \brief  Get variant value from type.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained.
 */
template <typename T, typename... Types>
T&& get(variant<Types...>&& v) {
  return ara::core::get<T>(std::move(v));
}

/*!
 * \brief  Get variant value from type.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained.
 */
template <typename T, typename... Types>
T const& get(variant<Types...> const& v) {
  return ara::core::get<T>(v);
}

/*!
 * \brief  Get variant value from type.
 * \param  v The variant which value shall be returned.
 * \return The value that is contained.
 */
template <typename T, typename... Types>
T const&& get(variant<Types...> const&& v) {
  return ara::core::get<T>(std::move(v));
}

/*!
 * \brief  Visit variant with visitor accepting all variant items.
 * \param  visitor The visitor, a functor accepting all variants.
 * \param  v The variant to visit.
 * \return The result returned by the functor.
 */
template <typename Visitor, typename... Types>
auto visit(Visitor&& visitor, variant<Types...>& v) -> decltype(ara::core::visit(std::forward<Visitor>(visitor), v)) {
  return ara::core::visit(std::forward<Visitor>(visitor), v);
}

/*!
 * \brief  Visit variant with visitor accepting all variant items.
 * \param  visitor The visitor, a functor accepting all variants.
 * \param  v The variant to visit.
 * \return The result returned by the functor.
 */
template <typename Visitor, typename... Types>
auto visit(Visitor&& visitor, variant<Types...> const& v)
    -> decltype(ara::core::visit(std::forward<Visitor>(visitor), v)) {
  return ara::core::visit(std::forward<Visitor>(visitor), v);
}

/*!
 * \brief  Visit variant with visitor accepting all variant items.
 * \param  visitor The visitor, a functor accepting all variants.
 * \param  v The variant to visit.
 * \return The result returned by the functor.
 */
template <typename Visitor, typename... Types>
auto visit(Visitor&& visitor, variant<Types...>&& v)
    -> decltype(ara::core::visit(std::forward<Visitor>(visitor), std::move(v))) {
  return ara::core::visit(std::forward<Visitor>(visitor), std::move(v));
}

/*!
 * \brief Swap overload for variant.
 */
template <typename... Types>
inline std::enable_if_t<vac::language::conjunction<std::is_move_constructible<Types>...>::value &&
                        vac::language::conjunction<vac::language::is_swappable<Types>...>::value>
swap(variant<Types...>& lhs, variant<Types...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_VARIANT_H_
