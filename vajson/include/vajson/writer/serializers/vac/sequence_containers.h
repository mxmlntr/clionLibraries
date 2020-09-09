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
/*!        \file  serializers/vac/sequence_containers.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_VAC_SEQUENCE_CONTAINERS_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_VAC_SEQUENCE_CONTAINERS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "ara/core/span.h"
#include "vac/container/static_list.h"
#include "vac/container/static_vector.h"

#include "vajson/writer/types/array_type.h"

namespace vajson {
namespace writer {
inline namespace serializers {
/*!
 * \brief Provides a serializer for array views of serializable elements
 * \tparam Serializer The serializer type
 * \tparam Value The Value type of the array view
 * \param s The serializer instance
 * \param view The array view to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Serializer, typename Value>
auto operator<<(Serializer s, ::ara::core::Span<Value> const& view) noexcept -> typename Serializer::Next {
  // VECTOR NL AutosarC++17_10-A18.9.2: MD_JSON_AutosarC++17_10-A18.9.2_serializer
  return std::move(s) << JArray(view);
}

/*!
 * \brief Provides a serializer for vectors of serializable elements
 * \tparam Serializer The serializer type
 * \tparam Value The Value type of the vector
 * \tparam Alloc The allocator type of the vector.
 * \param s The serializer instance
 * \param vec The vector to serialize
 * \returns the next serializer state
 *
 * \vprivate Vector component private
 */
template <typename Serializer, typename Value, typename Alloc>
auto operator<<(Serializer s, ::vac::container::StaticVector<Value, Alloc> const& vec) noexcept ->
    typename Serializer::Next {
  // VECTOR NL AutosarC++17_10-A18.9.2: MD_JSON_AutosarC++17_10-A18.9.2_serializer
  return std::move(s) << JArray(vec);
}

/*!
 * \brief Provides a serializer for list of serializable elements.
 * \tparam Serializer The serializer type.
 * \tparam Value The Value type of the list.
 * \tparam Alloc The allocator type of the list.
 * \param s The serializer instance.
 * \param lis The list to serialize.
 * \returns The next serializer state.
 *
 * \vprivate Vector component private
 */
template <typename Serializer, typename Value, typename Alloc>
auto operator<<(Serializer s, ::vac::container::StaticList<Value, Alloc> const& lis) noexcept ->
    typename Serializer::Next {
  // VECTOR NL AutosarC++17_10-A18.9.2: MD_JSON_AutosarC++17_10-A18.9.2_serializer
  return std::move(s) << JArray(lis);
}
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_VAC_SEQUENCE_CONTAINERS_H_
