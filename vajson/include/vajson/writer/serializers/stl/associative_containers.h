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
/*!        \file  serializers/stl/associative_containers.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STL_ASSOCIATIVE_CONTAINERS_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STL_ASSOCIATIVE_CONTAINERS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "vajson/writer/types/array_type.h"
#include "vajson/writer/types/object_type.h"

namespace vajson {
namespace writer {
inline namespace serializers {
/*!
 * \brief Provides a serializer for sets of serializable elements
 * \tparam Next The Next type for the Serializer
 * \tparam Value The Value type of the set
 * \tparam Cmp The comparison function
 * \tparam Alloc The allocator used for this container
 * \param s The serializer instance
 * \param set The set to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Next, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& s, std::set<Value, Cmp, Alloc> const& set) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JArray(set);
}

/*!
 * \brief Provides a serializer for unordered sets of serializable elements
 * \tparam Next The Next type for the Serializer
 * \tparam Value The Value type of the set
 * \tparam Hash The hash function used for separating object identity
 * \tparam Pred The predicate used to compare the hashes
 * \tparam Alloc The allocator used for this container
 * \param s The serializer instance
 * \param set The set to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Next, typename Value, typename Hash, typename Pred, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& s, std::unordered_set<Value, Hash, Pred, Alloc> const& set) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JArray(set);
}

/*!
 * \brief Provides a serializer for maps of serializable elements
 * \tparam Next The Next type for the Serializer
 * \tparam Key The key type of the map that must be convertible to a JKey
 * \tparam Value The Value type of the map
 * \tparam Cmp The comparison function
 * \tparam Alloc The allocator used for this container
 * \param s The serializer instance
 * \param map The map to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Next, typename Key, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& s, std::map<Key, Value, Cmp, Alloc> const& map) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JObject(map);
}

/*!
 * \brief Provides a serializer for unordered maps of serializable elements
 * \tparam Next The Next type for the Serializer
 * \tparam Key The key type of the map that must be convertible to a JKey
 * \tparam Value The Value type of the map
 * \tparam Hash The hash function used for separating object identity
 * \tparam Pred The predicate used to compare the hashes
 * \tparam Alloc The allocator used for this container
 * \param s The serializer instance
 * \param map The map to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Next, typename Key, typename Value, typename Hash, typename Pred, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& s, std::unordered_map<Key, Value, Hash, Pred, Alloc> const& map) noexcept
    -> typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JObject(map);
}

/*!
 * \brief Provides a serializer for multi maps of serializable elements
 * \tparam Next The Next type for the Serializer
 * \tparam Key The key type of the map that must be convertible to a JKey
 * \tparam Value The Value type of the map
 * \tparam Cmp The comparison function
 * \tparam Alloc The allocator used for this container
 * \param s The serializer instance
 * \param map The map to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Next, typename Key, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& s, std::multimap<Key, Value, Cmp, Alloc> const& map) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JObject(map);
}

/*!
 * \brief Provides a serializer for unordered multi maps of serializable elements
 * \tparam Next The Next type for the Serializer
 * \tparam Key The key type of the map that must be convertible to a JKey
 * \tparam Value The Value type of the map
 * \tparam Hash The hash function used for separating object identity
 * \tparam Pred The predicate used to compare the hashes
 * \tparam Alloc The allocator used for this container
 * \param s The serializer instance
 * \param map The map to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Next, typename Key, typename Value, typename Hash, typename Pred, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& s,
                std::unordered_multimap<Key, Value, Hash, Pred, Alloc> const& map) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JObject(map);
}
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STL_ASSOCIATIVE_CONTAINERS_H_
