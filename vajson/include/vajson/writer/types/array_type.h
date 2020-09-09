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
/*!        \file  array_type.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_TYPES_ARRAY_TYPE_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_TYPES_ARRAY_TYPE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <type_traits>
#include <utility>

#include "ara/core/string_view.h"

#include "vajson/writer/serializers/structures/serializer.h"
#include "vajson/writer/types/basic_types.h"

namespace vajson {
namespace writer {
inline namespace types {
/*!
 * \brief Serialize a homogeneous C++ range as a JSON array
 * \tparam Range The type of range
 *
 * \vprivate Vector component private
 */
template <typename Range, typename Fn>
class RangeSerializer final {
 public:
  /*!
   * \brief Initialize a Range serializer from range & function
   * \tparam Fn1 The function type for this serializer
   * \param range The range which to serialize
   * \param fn The function used to serialize
   * \attention Fn must not throw exceptions
   */
  template <typename Fn1 = Fn>
  RangeSerializer(Range const& range, Fn1&& fn) noexcept : container_{range}, function_{std::forward<Fn>(fn)} {}

  /*!
   * \brief Provides the implementation to serialize containers
   * \tparam Char The character type used for the output stream
   * \param as The array serializer
   * \returns nothing
   */
  template <typename AS = ArrayStart>
  auto operator()(AS as) const noexcept(noexcept(std::declval<Fn>()(std::declval<typename Range::value_type>())))
      -> void {
    // VECTOR NL AutosarC++17_10-A7.1.5: MD_JSON_AutosarC++17_10-A7.1.5_auto_it
    for (auto value : this->container_.get()) {
      as = std::move(as) << this->function_(value);
    }
  }

 private:
  /*!
   * \brief The container instance that is to be serialized
   */
  std::reference_wrapper<Range const> container_;

  /*!
   * \brief The function to serialize single items with
   */
  Fn function_;
};

/*!
 * \brief Serialize an ad-hoc defined Tuple as heterogeneous array
 * \tparam Fn The function type that defines the serialization
 *
 * \vprivate Vector component private
 */
template <typename Fn>
struct JArrayType final {
  /*!
   * \brief The wrapped function value
   */
  Fn fn;  // VECTOR SL AutosarC++17_10-M11.0.1: MD_JSON_AutosarC++17_10-M11.0.1_simple_struct
};

/*!
 * \brief Serialize a homogeneous C++ range as a JSON array
 * \tparam Range The type of range
 * \tparam Fn A special function to convert the single elements
 * \param range The range instance that is to be serialized
 * \param fn The function which is used to convert single elements to a serializable type
 * \returns a serializable JSON array
 *
 * \vpublic
 */
template <typename Range, typename Fn = IdSerializer<Range>>
auto JArray(Range const& range, Fn&& fn = IdSerializer<Range>{}) noexcept -> JArrayType<RangeSerializer<Range, Fn>> {
  return {RangeSerializer<Range, Fn>{range, std::forward<Fn>(fn)}};
}

// VECTOR NC AutosarC++17_10-A13.3.1: MD_JSON_AutosarC++17_10-A13.3.1_enable_if
/*!
 * \brief Serialize an ad-hoc defined Tuple as heterogeneous array
 * \param fn A function that receives a TupleSerializer object and returns it again.
 * \details The function can be used to define a tuple by adding values
 * \returns the serializable Tuple type
 *
 * \vpublic
 */
template <typename Fn>
auto JArray(Fn&& fn) noexcept -> JArrayType<Fn> {
  return {std::forward<Fn>(fn)};
}
}  // namespace types
}  // namespace writer
}  // namespace vajson
#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_TYPES_ARRAY_TYPE_H_
