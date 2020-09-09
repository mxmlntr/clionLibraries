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
/*!        \file  cpp14_backport.h
 *        \brief  Contains useful backports of STL features from C++14 to C++11.
 *
 *      \details  The C++14 backported features also contain C++11 backports.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_CPP14_BACKPORT_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_CPP14_BACKPORT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <memory>
#include <type_traits>
#include <utility>

namespace vac {
namespace language {

/*!
 * \brief Equivalent to C++14 std::common_type_t.
 */
template <typename... Types>
using common_type_t = typename std::common_type<Types...>::type;

/*!
 * \brief Equivalent to C++14 std::make_signed_t.
 */
template <typename T>
using make_signed_t = typename std::make_signed<T>::type;

/*!
 * \brief Equivalent to C++14 std::decay_t.
 */
template <typename T>
using decay_t = typename std::decay<T>::type;

/*!
 * \brief Equivalent to C++14 std::enable_if_t.
 */
template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

/*!
 * \brief  Re-implementation of C++14's std::result_of_t.
 * \tparam U Callable.
 */
template <typename U>
using result_of_t = typename std::result_of<U>::type;

/*!
 * \brief  Equivalent to C++14 std::make_unique for non-arrays.
 * \param  args Arguments to the pointee.
 * \return A pointer to the created type.
 */
template <typename T, typename... Args, typename = enable_if_t<!std::is_array<T>::value>>
std::unique_ptr<T> make_unique(Args&&... args) {
  /* VECTOR Next Line AutosarC++17_10-A18.5.2: MD_VAC_A18.5.2_makeUnique */
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/*!
 * \brief  Equivalent to C++14 std::make_unique for arrays.
 * \param  size Array Size to allocate.
 * \return A pointer to the created array.
 */
template <typename T, typename = enable_if_t<std::is_array<T>::value>>
std::unique_ptr<T> make_unique(std::size_t size) {
  // VECTOR NL AutosarC++17_10-A18.5.2, AutosarC++17_10-A18.1.4: MD_VAC_A18.5.2_makeUnique, MD_VAC_A18.1.4_uniquePtr
  return std::unique_ptr<T>(new typename std::remove_extent<T>::type[size]());
}

/*!
 * \brief  Equivalent to C++14 std::max.
 * \param  a The first option.
 * \param  b The second option.
 * \return b if it is larger than a else returns a.
 */
template <typename T>
constexpr T max(T a, T b) {  // NOLINT(build/include_what_you_use)
  return (a < b) ? b : a;
}

/*!
 * \brief  Equivalent to C++14 std::min.
 * \param  a The first option.
 * \param  b The second option.
 * \return a if it is smaller than a else returns b.
 */
template <typename T>
constexpr T min(T a, T b) {  // NOLINT(build/include_what_you_use)
  return (a < b) ? a : b;
}

/*!
 * \brief  Equivalent to C++14 std::integer_sequence.
 * \tparam T Value type.
 * \tparam Ints Sequence of Ts.
 * \trace  CREQ-166857
 */
template <typename T, T... Ints>
/* VECTOR Next Line AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_compileTimeResolution */
struct integer_sequence {
  /*!
   * \brief Type of contained element.
   */
  using value_type = T;

  /*!
   * \brief Type of the size field.
   */
  using size_type = std::size_t;

  /*!
   * \brief  Get the length of the integer sequence.
   * \return The size of the integer sequence.
   */
  static constexpr size_type size() { return sizeof...(Ints); }
};

/*!
 * \brief   Equivalent to C++14 std::make_integer_sequence.
 * \details Recursively creates a sequence of type T so as to [T{0}, T{1}, T{2}, ..., T{N-1}].
 * \tparam  T Value type.
 * \tparam  N Value count.
 * \tparam  Ints Sequence of Ts.
 * \trace   CREQ-166857
 */
template <typename T, std::size_t N, T... Ints>
/* VECTOR Next Line AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_compileTimeResolution */
struct make_integer_sequence /*! \cond */ : make_integer_sequence<T, N - 1, N - 1, Ints...> /*! \endcond */ {};

/*!
 * \brief  Recursion anchor for make_integer_sequence.
 * \tparam T Value type.
 * \tparam Ints Sequence of Ts.
 * \trace  CREQ-166857
 */
template <typename T, T... Ints>
struct make_integer_sequence<T, 0, Ints...> : integer_sequence<T, Ints...> {};

/*!
 * \brief  Equivalent to C++14 std::index_sequence.
 * \tparam Ints Sequence of std::size_t.
 * \trace  CREQ-166857
 */
template <std::size_t... Ints>
using index_sequence = integer_sequence<std::size_t, Ints...>;

/*!
 * \brief  Equivalent to C++14 std::make_index_sequence.
 * \tparam N Value count.
 * \trace  CREQ-166857
 */
template <std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;

/*!
 * \brief  Equivalent to C++14 std::remove_reference_t.
 * \tparam T Value type.
 * \trace  CREQ-166856
 */
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

/*!
 * \brief  Equivalent to C++14 std::add_const_t.
 * \tparam T Value type.
 * \trace  CREQ-171849
 */
template <typename T>
using add_const_t = typename std::add_const<T>::type;

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_CPP14_BACKPORT_H_
