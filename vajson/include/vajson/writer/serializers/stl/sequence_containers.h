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
/*!        \file  serializers/stl/sequence_containers.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STL_SEQUENCE_CONTAINERS_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STL_SEQUENCE_CONTAINERS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <array>
#include <deque>
#include <type_traits>
#include <utility>
#include <vector>

#include "vajson/writer/types/array_type.h"

namespace vajson {
namespace writer {
inline namespace serializers {
/*!
 * \brief Provides a serializer for arrays of serializable elements
 * \tparam Next The Next type for the Serializer
 * \tparam Value The Value type of the array
 * \tparam N The size of the array
 * \param s The serializer instance
 * \param array The array to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Next, typename Value, std::size_t N>
auto operator<<(GenericValueSerializer<Next>&& s, std::array<Value, N> const& array) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JArray(array);
}

/*!
 * \brief Provides a serializer for vectors of serializable elements
 * \tparam Next The Next type for the Serializer
 * \tparam Value The Value type of the vector
 * \tparam Alloc The allocator used for this container
 * \param s The serializer instance
 * \param vector The vector to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Next, typename Value, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& s, std::vector<Value, Alloc> const& vector) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JArray(vector);
}

/*!
 * \brief Provides a serializer for deques of serializable elements
 * \tparam Next The Next type for the Serializer
 * \tparam Value The Value type of the deque
 * \tparam Alloc The allocator used for this container
 * \param s The serializer instance
 * \param deque The deque to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Next, typename Value, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& s, std::deque<Value, Alloc> const& deque) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JArray(deque);
}
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STL_SEQUENCE_CONTAINERS_H_
