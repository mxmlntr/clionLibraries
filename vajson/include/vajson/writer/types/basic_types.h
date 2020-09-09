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
/*!        \file  basic_types.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_TYPES_BASIC_TYPES_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_TYPES_BASIC_TYPES_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <string>
#include <type_traits>
#include <utility>

#include "ara/core/string_view.h"
#include "vac/container/basic_string.h"
#include "vac/container/c_string_view.h"

#include "vajson/util/types.h"

namespace vajson {
namespace writer {
inline namespace types {
/*!
 * \brief Serialize a Null value
 *
 * \vprivate Vector component private
 */
struct JNullType final {};

/*!
 * \brief Serialize a Null value
 * \returns the serializable null type
 *
 * \vpublic
 */
constexpr inline JNullType JNull() noexcept { return {}; }

/*!
 * \brief Serialize a Bool value
 *
 * \vprivate Vector component private
 */
struct JBoolType final {
  /*!
   * \brief The wrapped bool value
   */
  bool value;
};

/*!
 * \brief Serialize a Bool value
 * \param b The bool to serialize
 * \returns the serializable bool type
 *
 * \vpublic
 */
constexpr inline JBoolType JBool(bool b) noexcept { return {b}; }

/*!
 * \brief Serialize a Key value
 *
 * \vprivate Vector component private
 */
class JKeyType final {
 public:
  /*!
   * \brief Construct a Key value
   * \param s The key to serialize
   */
  explicit constexpr JKeyType(vac::container::string_view s) noexcept : value_(s) {}

  /*!
   * \brief Construct a Key value
   * \param s The key to serialize
   */
  explicit JKeyType(std::string const& s) noexcept : value_(s.data(), s.size()) {}

  /*!
   * \brief Returns the contained value
   * \return the contained value
   */
  ara::core::StringView GetValue() const { return this->value_; }

 private:
  /*!
   * \brief The wrapped string value
   */
  ara::core::StringView value_;
};

/*!
 * \brief Serialize a Key value
 * \param s The key to serialize
 * \returns the serializable key type
 *
 * \vpublic
 */
constexpr auto JKey(ara::core::StringView s) noexcept -> JKeyType { return JKeyType{s}; }

/*!
 * \brief Serialize a Key value
 * \param s The key to serialize
 * \returns the serializable key type
 *
 * \vpublic
 */
constexpr auto JKey(vac::container::CStringView s) noexcept -> JKeyType { return JKeyType{ara::core::StringView(s)}; }

/*!
 * \brief Serialize a Key value
 * \param s The key to serialize
 * \returns the serializable key type
 *
 * \vpublic
 */
constexpr auto JKey(std::string const& s) noexcept -> JKeyType { return JKeyType{vac::container::string_view(s)}; }

/*!
 * \brief Serialize a Number value
 *
 * \vprivate Vector component private
 */
template <typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value>::type>
class JNumberType final {
 public:
  /*!
   * \brief Constructor from a number
   * \param num The number to write
   */
  constexpr explicit JNumberType(N num) noexcept : value_(num) {}

  /*!
   * \brief Returns the contained value
   * \return the contained value
   */
  N GetValue() const noexcept { return this->value_; }

 private:
  /*!
   * \brief The wrapped number value
   */
  N value_;
};

/*!
 * \brief Serialize a Number value
 *
 * \vprivate Vector component private
 */
template <>
class JNumberType<char8_t> final {
 public:
  /*!
   * \brief Constructor from character
   * \param num The number to write
   */
  constexpr explicit JNumberType(char8_t num) noexcept : value_(static_cast<std::int32_t>(num)) {}

  /*!
   * \brief Returns the contained value
   * \return the contained value
   */
  std::int32_t GetValue() const noexcept { return this->value_; }

 private:
  /*!
   * \brief The wrapped number value
   */
  std::int32_t value_;
};

/*!
 * \brief Serialize a Number value
 *
 * \vprivate Vector component private
 */
template <>
class JNumberType<std::uint8_t> final {
 public:
  /*!
   * \brief Constructor from character
   * \param num The number to write
   */
  constexpr explicit JNumberType(std::uint8_t num) noexcept : value_(static_cast<std::uint32_t>(num)) {}

  /*!
   * \brief Returns the contained value
   * \return the contained value
   */
  std::uint32_t GetValue() const noexcept { return this->value_; }

 private:
  /*!
   * \brief The wrapped number value
   */
  std::uint32_t value_;
};

/*!
 * \brief Serialize a Number value
 *
 * \vprivate Vector component private
 */
template <>
class JNumberType<std::int8_t> final {
 public:
  /*!
   * \brief Constructor from character
   * \param num The number to write
   */
  constexpr explicit JNumberType(std::int8_t num) noexcept : value_(static_cast<std::int32_t>(num)) {}

  /*!
   * \brief Returns the contained value
   * \return the contained value
   */
  std::int32_t GetValue() const noexcept { return this->value_; }

 private:
  /*!
   * \brief The wrapped number value
   */
  std::int32_t value_;
};

/*!
 * \brief Serialize a Number value
 * \param n The number to serialize
 * \returns the serializable number type
 *
 * \vpublic
 */
template <typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value>::type>
constexpr auto JNumber(N n) noexcept -> JNumberType<N> {
  return JNumberType<N>{n};
}

/*!
 * \brief Serialize a String value
 *
 * \vprivate Vector component private
 */
class JStringType final {
 public:
  /*!
   * \brief Construct a String value
   * \param s The string to serialize
   */
  constexpr explicit JStringType(vac::container::string_view s) noexcept : value_(s) {}

  /*!
   * \brief Construct a String value
   * \param s The string to serialize
   */
  template <typename CharTraits, typename Alloc>
  constexpr explicit JStringType(vac::container::basic_string<char8_t, CharTraits, Alloc> const& s) noexcept
      : value_(s.data(), s.size()) {}

  /*!
   * \brief Returns the contained value
   * \return the contained value
   */
  ara::core::StringView GetValue() const { return this->value_; }

 private:
  /*!
   * \brief The wrapped string value
   */
  ara::core::StringView value_;
};

/*!
 * \brief Serialize a String value
 * \param s The string to serialize
 * \returns the serializable string type
 *
 * \vpublic
 */
constexpr auto JString(ara::core::StringView s) noexcept -> JStringType { return JStringType(s); }

/*!
 * \brief Serialize a String value
 * \tparam Char The character type of the string
 * \tparam CharTraits The char traits to use
 * \tparam Alloc The allocator used for the string
 * \param s The string to serialize
 * \returns the serializable string type
 *
 * \vpublic
 */
template <typename CharTraits, typename Alloc>
constexpr auto JString(vac::container::basic_string<char8_t, CharTraits, Alloc> const& s) noexcept -> JStringType {
  return JStringType(s);
}

/*!
 * \brief Function object used to serialize predefined serializers
 * \tparam Container The type of container to serialize
 *
 * \vprivate Vector component private
 */
template <typename Container>
class IdSerializer {
 public:
  /*!
   * \brief The value type of the container
   */
  using value_type = typename Container::value_type;
  /*!
   * \brief The call operator that returns the unchanged value
   * \param v the value to serialize
   * \returns the unchanged value
   */
  template <typename Value = value_type>
  auto operator()(Value&& v) const noexcept -> Value {
    return std::forward<Value>(v);
  }
};
}  // namespace types
}  // namespace writer
}  // namespace vajson
#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_TYPES_BASIC_TYPES_H_
