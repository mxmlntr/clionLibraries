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
/*!        \file  byte.h
 *        \brief  Backport of the C++17 type std::byte.
 *
 *      \details  byte is a distinct type that implements the concept of byte as specified in the C++ language
 *                definition.
 *                Like char and unsigned char, it can be used to access raw memory occupied by other objects (object
 *                representation), but unlike those types, it is not a character type and is not an arithmetic type. A
 *                byte is only a collection of bits, and the only operators defined for it are the bitwise ones.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_BYTE_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_BYTE_H_

#include <cstdint>
#include <stdexcept>

#include "vac/language/cpp14_backport.h"
#include "vac/language/throw_or_terminate.h"

namespace vac {
namespace language {
/*!
 * \brief Equivalent to C++17 std::byte.
 * \trace CREQ-204265
 */
enum class byte : std::uint8_t {};

/*!
 * \brief  Cast any integer type to byte.
 * \tparam IntegerType The integer type to cast.
 * \param  b The integer to cast (must be smaller or equal to 0xFF).
 * \return The value as a byte.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte to_byte(IntegerType b) noexcept {
  return static_cast<byte>(b);
}

inline namespace literals {
/*!
 * \brief  Creates a byte from a literal. The target variable must be a constexpr.
 *         Without constexpr throws std::logic_error in run time instead of compile time.
 * \param  i The integer literal from which to create the byte.
 * \return The value as a byte.
 * \throws std::logic_error if constexpr literal is too large.
 */
constexpr byte operator"" _byte(unsigned long long int i) {  // NOLINT(runtime/int)
  return (i <= 0xFF) ? to_byte(i)
                     : (vac::language::ThrowOrTerminate<std::logic_error>("Literal must fit in a byte"), to_byte(0));
}
}  // namespace literals

/*!
 * \brief Cast byte to any other integer type.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr IntegerType to_integer(byte b) noexcept {
  return static_cast<IntegerType>(b);
}

/*!
 * \brief Bitwise left shift.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte operator<<(byte b, IntegerType shift) noexcept {
  return static_cast<byte>(static_cast<std::uint8_t>(b) << shift);
}

/*!
 * \brief Bitwise right shift.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte operator>>(byte b, IntegerType shift) noexcept {
  return static_cast<byte>(static_cast<std::uint8_t>(b) >> shift);
}

/*!
 * \brief Bitwise left shift assignment.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte& operator<<=(byte& b, IntegerType shift) noexcept {
  return b = b << shift;
}

/*!
 * \brief Bitwise right shift assignment.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte& operator>>=(byte& b, IntegerType shift) noexcept {
  return b = b >> shift;
}

/*!
 * \brief Bitwise or operator.
 */
constexpr byte operator|(byte l, byte r) noexcept {
  return static_cast<byte>(static_cast<std::uint8_t>(l) | static_cast<std::uint8_t>(r));
}
/*!
 * \brief Bitwise and operator.
 */
constexpr byte operator&(byte l, byte r) noexcept {
  return static_cast<byte>(static_cast<std::uint8_t>(l) & static_cast<std::uint8_t>(r));
}
/*!
 * \brief Bitwise xor operator.
 */
constexpr byte operator^(byte l, byte r) noexcept {
  return static_cast<byte>(static_cast<std::uint8_t>(l) ^ static_cast<std::uint8_t>(r));
}
/*!
 * \brief Bitwise not operator.
 */
constexpr byte operator~(byte b) noexcept { return static_cast<byte>(~static_cast<std::uint8_t>(b)); }

// TODO(AMSR-23447 / HUN-136): Should be constexpr after change to C++14
/*!
 * \brief Bitwise or assignment.
 */
inline byte& operator|=(byte& l, byte r) noexcept { return l = l | r; }
/*!
 * \brief Bitwise and assignment.
 */
inline byte& operator&=(byte& l, byte r) noexcept { return l = l & r; }
/*!
 * \brief Bitwise xor assignment.
 */
inline byte& operator^=(byte& l, byte r) noexcept { return l = l ^ r; }

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_BYTE_H_
