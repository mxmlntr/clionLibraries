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
/*!        \file  object_type.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_TYPES_OBJECT_TYPE_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_TYPES_OBJECT_TYPE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <type_traits>
#include <utility>

#include "ara/core/string_view.h"

#include "vajson/util/types.h"

#include "vajson/writer/serializers/structures/serializer.h"
#include "vajson/writer/types/basic_types.h"

namespace vajson {
namespace writer {
inline namespace types {
/*!
 * \brief Function object used to serialize predefined key serializers
 * \tparam Char The charater encoding used for the writer
 *
 * \vprivate Vector component private
 */
class DefaultKeySerializer {
 public:
  /*!
   * \brief The call operator that returns the transformed value
   * \param value the value to serialize as a key
   * \returns the key value
   */
  template <typename T>
  auto operator()(T const& value) const noexcept -> JKeyType {
    static_assert(std::is_constructible<JKeyType, T>::value, "Keys must be convertible to a JKey");
    return JKeyType{value};
  }
};

/*!
 * \brief Serialize a homogeneous C++ pair-range as a JSON array
 * \tparam Range The type of range to serialize
 * \tparam Char The character encoding for the serializer
 * \tparam KeyFn The key function type
 * \tparam ValueFn The value function type
 *
 * \vprivate Vector component private
 */
template <typename Range, typename KeyFn, typename ValueFn>
class PairRangeSerializer final {
 public:
  /*!
   * \brief Constructs a PairRangeSerializer
   * \tparam KeyFn1 The key function type
   * \tparam ValueFn1 The value function type
   * \param range The Range to serialize
   * \param key_fn The key function used to serialize
   * \param value_fn The value function used to serialize
   */
  template <typename KeyFn1 = KeyFn, typename ValueFn1 = ValueFn>
  PairRangeSerializer(Range const& range, KeyFn1&& key_fn, ValueFn1&& value_fn) noexcept
      : map_{range}, key_function_{std::forward<KeyFn>(key_fn)}, value_function_{std::forward<ValueFn>(value_fn)} {}

  /*!
   * \brief Provides the implementation to serialize containers
   * \tparam Char The character type used for the output stream
   * \param os The array serializer
   * \returns The serializer state after serializing the range
   */
  template <typename KS = KeySerializer>
  auto operator()(KS os) const -> KS {
    // VECTOR NL AutosarC++17_10-A7.1.5: MD_JSON_AutosarC++17_10-A7.1.5_auto_it
    for (auto const& pair : this->map_.get()) {
      ObjectSerializerValue osv{std::move(os) << this->key_function_(pair.first)};
      os = std::move(osv) << this->value_function_(pair.second);
    }
    return std::move(os);
  }

 private:
  /*!
   * \brief The container instance that is to be serialized
   */
  std::reference_wrapper<Range const> map_;

  /*!
   * \brief The function to serialize single keys with
   */
  KeyFn key_function_;

  /*!
   * \brief The function to serialize single values with
   */
  ValueFn value_function_;
};

/*!
 * \brief Serialize an object value
 *
 * \vprivate Vector component private
 */
template <typename Fn>
struct JObjectType final {
  /*!
   * \brief The function that defines the serializable object
   */
  Fn fn;  // VECTOR SL AutosarC++17_10-M11.0.1: MD_JSON_AutosarC++17_10-M11.0.1_simple_struct
};

/*!
 * \brief Serialize an object value
 * \param fn The function that defines the Object
 * \returns a serializable object
 *
 * \vpublic
 */
template <typename Fn, typename = std::enable_if_t<!std::is_lvalue_reference<Fn>::value>>
auto JObject(Fn&& fn) noexcept -> JObjectType<Fn> {
  return {std::forward<Fn>(fn)};
}

// VECTOR NC AutosarC++17_10-A13.3.1: MD_JSON_AutosarC++17_10-A13.3.1_enable_if
/*!
 * \brief Serialize a homogeneous C++ pair-range (e.g. a map) as a JSON object
 * \tparam Range The type of range
 * \tparam KeyFn The serializer function type for keys
 * \tparam ValueFn The serializer function type for values
 * \param range The range instance to be serialized
 * \param key_fn The serializer function for keys
 * \param value_fn The serializer function for values
 * \returns a serializable JSON object
 *
 * \vpublic
 */
template <typename Range, typename KeyFn = DefaultKeySerializer, typename ValueFn = IdSerializer<Range>>
auto JObject(Range const& range, KeyFn&& key_fn = DefaultKeySerializer{},
             ValueFn&& value_fn = IdSerializer<Range>{}) noexcept
    -> JObjectType<PairRangeSerializer<Range, KeyFn, ValueFn>> {
  return {
      PairRangeSerializer<Range, KeyFn, ValueFn>{range, std::forward<KeyFn>(key_fn), std::forward<ValueFn>(value_fn)}};
}
}  // namespace types
}  // namespace writer
}  // namespace vajson
#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_TYPES_OBJECT_TYPE_H_
