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
/*!        \file  serializers/stl/primitives.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STL_PRIMITIVES_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STL_PRIMITIVES_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <string>
#include <type_traits>
#include <utility>

#include "ara/core/string_view.h"

#include "vajson/util/types.h"
#include "vajson/writer/types/basic_types.h"

namespace vajson {
namespace writer {
inline namespace serializers {
/*!
 * \brief Forward declaration for the GenericValueSerializer
 * \tparam Return The type returned after using operator<<
 */
template <typename Return>
class GenericValueSerializer;

/*!
 * \brief Serialize a null value directly from a nullptr literal
 * \tparam Next The Next type for the Serializer
 * \param s The serializer to write into
 * \returns the successor serializer
 *
 * \vpublic
 */
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& s, std::nullptr_t) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JNull();
}

/*!
 * \brief Serialize a value directly from a pointer
 * \details Serializes null literal if the pointer is nullptr
 * \tparam Next The Next type for the Serializer
 * \param s The serializer to write into
 * \param ptr The pointer whose value to serialize
 * \returns the successor serializer
 *
 * \vpublic
 */
template <typename Next, typename T>
auto operator<<(GenericValueSerializer<Next>&& s, T const* ptr) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return (ptr == nullptr) ? (std::move(s) << JNull()) : (std::move(s) << *ptr);
}

/*!
 * \brief Serialize a bool value directly
 * \tparam Next The Next type for the Serializer
 * \param s The serializer to write into
 * \param b the bool value to serialize
 * \returns the successor serializer
 *
 * \vpublic
 */
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& s, bool b) noexcept -> typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JBool(b);
}

/*!
 * \brief Serialize a number value directly
 * \tparam Next The Next type for the Serializer
 * \param s The serializer to write into
 * \param number the number to serialize
 * \returns the successor serializer
 *
 * \vpublic
 */
template <typename Next, typename N, typename = std::enable_if_t<std::is_arithmetic<N>::value>>
auto operator<<(GenericValueSerializer<Next>&& s, N number) noexcept -> typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JNumber(number);
}

/*!
 * \brief Serialize a string value directly
 * \tparam Next The Next type for the Serializer
 * \param s The serializer to write into
 * \param string the string to serialize
 * \returns the successor serializer
 *
 * \vpublic
 */
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& s, std::string const& string) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(s) << JString(string);
}
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STL_PRIMITIVES_H_
