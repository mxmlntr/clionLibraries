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
/*!        \file  serializers/vac/primitives.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_VAC_PRIMITIVES_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_VAC_PRIMITIVES_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "ara/core/string_view.h"
#include "vac/container/c_string_view.h"
#include "vac/language/byte.h"

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
 * \brief Serialize a string value directly
 * \tparam Next The Next type for the Serializer
 * \param ser The serializer to write into
 * \param str the string to serialize
 * \returns the successor serializer
 *
 * \vpublic
 */
template <typename Next>
auto operator<<(GenericValueSerializer<Next> ser, ::vac::container::string_view str) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(ser) << JString(str);
}

/*!
 * \brief Serialize a string value directly
 * \tparam Next The Next type for the Serializer
 * \param ser The serializer to write into
 * \param str the string to serialize
 * \returns the successor serializer
 *
 * \vprivate Vector component internal
 */
template <typename Next>
auto operator<<(GenericValueSerializer<Next> ser, ::vac::container::CStringView str) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(ser) << JString(str);
}

/*!
 * \brief Serialize a vac::language::byte value directly
 * \tparam Next The Next type for the Serializer
 * \param ser The serializer to write into
 * \param byte the byte to serialize
 * \returns the successor serializer
 *
 * \vpublic
 */
template <typename Next, typename Char = ::vac::language::byte>
auto operator<<(GenericValueSerializer<Next> ser, ::vac::language::byte byte) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(ser) << ::vac::language::to_integer<std::uint16_t>(byte);
}
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_VAC_PRIMITIVES_H_
