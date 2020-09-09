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
/*!        \file ara/core/utility.h
 *        \brief SWS core type generic helpers.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_UTILITY_H_
#define LIB_VAC_INCLUDE_ARA_CORE_UTILITY_H_

#include <initializer_list>
#include <utility>

namespace ara {
namespace core {
/*!
 * \brief   Denote an operation to be performed in-place.
 * \details An instance of this type can be passed to certain constructors of ara::core::Optional to denote
 *          the intention that construction of the contained type shall be done in-place, i.e. without any
 *          copying taking place.
 * \trace   SPEC-7552598
 * \trace   CREQ-LibVac-Utility001
 */
struct in_place_t {};

/*!
 * \brief The singleton instance of in_place_t.
 * \trace SPEC-7552600
 * \trace CREQ-LibVac-Utility001
 */
constexpr in_place_t in_place{};

/*!
 * \brief   Denote a type-distinguishing operation to be performed in-place.
 * \details An instance of this type can be passed to certain constructors of ara::core::Variant to denote
 *          the intention that construction of the contained type shall be done in-place, i.e. without any
 *          copying taking place.
 * \trace   SPEC-7552601
 * \trace   CREQ-LibVac-Utility001
 */
template <typename T>
struct in_place_type_t {};

/*!
 * \brief   Denote an index-distinguishing operation to be performed in-place.
 * \details An instance of this type can be passed to certain constructors of ara::core::Variant to denote
 *          the intention that construction of the contained type shall be done in-place, i.e. without any
 *          copying taking place.
 * \trace   SPEC-7552603
 * \trace   CREQ-LibVac-Utility001
 */
template <std::size_t I>
struct in_place_index_t {};

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief   Return a pointer to the block of memory that contains the elements of a container
 * \tparam  Container The type of the container
 * \param   c An instance of Container
 * \return  A pointer to the block of memory that contains the elements of a container
 * \trace   SPEC-7552605
 * \trace   CREQ-LibVac-Utility001
 * \vpublic
 */
template <typename Container>
constexpr auto data(Container& c) -> decltype(c.data()) {
  return c.data();
}

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief   Return a const_pointer to the block of memory that contains the elements of a container.
 * \tparam  Container The type of the container
 * \param   c An instance of Container
 * \return  A const_pointer to the block of memory that contains the elements of a container
 * \trace   SPEC-7552606
 * \trace   CREQ-LibVac-Utility001
 * \vpublic
 */
template <typename Container>
constexpr auto data(Container const& c) -> decltype(c.data()) {
  return c.data();
}

// VECTOR Next Construct AutosarC++17_10-A18.1.1: MD_VAC_A18.1.1_cStyleArraysShouldNotBeUsed
// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief   Return a pointer to the block of memory that contains the elements of a raw array
 * \tparam  T The type of array elements
 * \tparam  N The number of elements in the array
 * \param   array Reference to a raw array
 * \return  A pointer to the block of memory that contains the elements of a raw array
 * \trace   SPEC-7552607
 * \trace   CREQ-LibVac-Utility001
 * \vpublic
 */
template <typename T, std::size_t N>
constexpr T const* data(T (&array)[N]) noexcept {
  return array;
}

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief   Return a pointer to the block of memory that contains the elements of a std::initializer_list
 * \tparam  E The type of elements in the std::initializer_list
 * \param   il The std::initializer_list
 * \return  A pointer to the block of memory that contains the elements of a std::initializer_list
 * \trace   SPEC-7552608
 * \trace   CREQ-LibVac-Utility001
 * \vpublic
 */
template <typename E>
constexpr E const* data(std::initializer_list<E> il) noexcept {
  return il.begin();
}

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief   Return the size of a container
 * \tparam  Container A type with a data() method
 * \param   c An instance of Container
 * \return  The size of a container
 * \trace   SPEC-7552609
 * \trace   CREQ-LibVac-Utility001
 * \vpublic
 */
template <typename Container>
constexpr auto size(Container const& c) -> decltype(c.size()) {
  return c.size();
}

// VECTOR Next Construct AutosarC++17_10-A18.1.1: MD_VAC_A18.1.1_cStyleArraysShouldNotBeUsed
// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief   Return the size of a raw array
 * \tparam  T The type of array elements
 * \tparam  N The number of elements in the array
 * \param   array Reference to a raw array
 * \return  The size of a raw array.
 * \trace   SPEC-7552610
 * \trace   CREQ-LibVac-Utility001
 * \vpublic
 */
template <typename T, std::size_t N>
constexpr auto size(T const (&array)[N]) noexcept -> std::size_t {
  static_cast<void>(array);
  return N;
}

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief   Return whether the given container is empty
 * \tparam  Container A type with a empty() method
 * \param   c An instance of Container
 * \return  Whether the given container is empty
 * \trace   SPEC-7552611
 * \trace   CREQ-LibVac-Utility001
 * \vpublic
 */
template <typename Container>
constexpr auto empty(Container const& c) -> decltype(c.empty()) {
  return c.empty();
}

// VECTOR Next Construct AutosarC++17_10-A18.1.1: MD_VAC_A18.1.1_cStyleArraysShouldNotBeUsed
// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief   Return whether the given raw array is empty
 * \tparam  T The type of array elements
 * \tparam  N The number of elements in the array
 * \param   array The raw array
 * \return  As raw arrays cannot have zero elements in C++, this function always returns false
 * \trace   SPEC-7552612
 * \trace   CREQ-LibVac-Utility001
 * \vpublic
 */
template <typename T, std::size_t N>
constexpr auto empty(T const (&array)[N]) noexcept -> bool {
  static_cast<void>(array);
  return false;
}

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief   Return whether the given std::initializer_list is empty
 * \tparam  E The type of elements in the std::initializer_list
 * \param   il The std::initializer_list
 * \return  Whether the given std::initializer_list is empty
 * \trace   SPEC-7552613
 * \trace   CREQ-LibVac-Utility001
 * \vpublic
 */
template <typename E>
constexpr auto empty(std::initializer_list<E> il) noexcept -> bool {
  return il.size() == 0;
}

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_UTILITY_H_
