/**********************************************************************************************************************
 *  COPYRIGHT
 * -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2019 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 * -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 * -----------------------------------------------------------------------------------------------------------------*/
/*!       \file     number.h
 *        \brief    TODO
 *
 *        \details  TODO
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_UTIL_NUMBER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_UTIL_NUMBER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <array>
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>

#include "ara/core/optional.h"
#include "ara/core/result.h"
#include "ara/core/string_view.h"
#include "vac/container/string_literals.h"
#include "vac/language/byte.h"
#include "vac/language/compile_time.h"

#include "vajson/util/json_error_domain.h"
#include "vajson/util/types.h"

namespace vajson {
namespace util {

/*!
 * \brief Enum for setting different number bases
 * \details kZeroOnly Internal state that is mapped on decimal for the std conversion functions
 *
 * \vprivate
 */
enum class NumberBase : std::uint8_t {
  kAutoDetect = 0,
  kBinary = 2,
  kOctal = 8,
  kDecimal = 10,
  kHex = 16,
  kZeroOnly = 0
};

namespace internal {

/*!
 * \brief Resets the value of errno
 */
inline auto ResetErrno() -> void {
  // VECTOR NL AutosarC++17_10-M19.3.1_errno: MD_JSON_AutosarC++17_10-M19.3.1_errno
  errno = 0;
}

/*!
 * \brief Returns the value of errno
 */
inline auto GetErrno() -> PosixInteger {
  // VECTOR NL AutosarC++17_10-M19.3.1_errno: MD_JSON_AutosarC++17_10-M19.3.1_errno
  return errno;
}

/*!
 * \brief Checks if the character is a digit
 * \param ch the character to check
 * \param base a switch that enables hexadecimal digits
 * \returns true if the value is a digit
 * \vprivate
 */
inline auto IsDigit(char8_t ch, util::NumberBase base) noexcept -> bool {
  bool is_successful;
  switch (base) {
    case util::NumberBase::kOctal:
      is_successful = (ch >= '0') && (ch <= '7');
      break;
    case util::NumberBase::kDecimal:
      is_successful = std::isdigit(ch);
      break;
    case util::NumberBase::kHex:
      is_successful = std::isxdigit(ch);
      break;
    default:
      is_successful = false;
      break;
  }
  return is_successful;
}

/*!
 * \brief Check if type is an int (does not include bool!)
 */
template <typename T>
using IsInt =
    typename vac::language::compile_time::all<!std::is_same<T, bool>::value, std::is_integral<T>::value>::type;

/*!
 * \brief Typedef for longlong
 * \details Used as return type for strtoll, is transformed to fixed-size type later
 */
using SignedLL = long long int;  // NOLINT(runtime/int)

/*!
 * \brief Typedef for unsigned longlong
 * \details Used as return type for strtoull, is transformed to fixed-size type later
 */
using UnsignedLL = unsigned long long int;  // NOLINT(runtime/int)

/*!
 * \brief Parse strings into numbers
 * \pre This class assumes that all leading whitespace has been stripped
 */
class Parser {
  /*!
   * \brief a string view specialized onto the current encoding
   */
  using StringView = ara::core::StringView;

 public:
  /*!
   * \brief Constructor taking a manual set base
   */
  explicit Parser(NumberBase base) noexcept : base_{base}, end_{nullptr} {}
  /*!
   * \brief Get a pointer beyond the last parsed character
   * \returns a pointer beyond the last parsed character
   */
  auto End() const noexcept -> char8_t const* { return end_; }

  /*!
   * \brief Parses a number as a long long
   * \param view the string view to parse
   * \return the number as long long
   */
  auto LongLong(StringView view) noexcept -> SignedLL {
    internal::ResetErrno();
    // VECTOR NL AutosarC++17_10-A3.9.1: MD_JSON_AutosarC++17_10-A3.9.1_using
    return std::strtoll(view.data(), &this->end_, static_cast<int>(this->base_));
  }

  /*!
   * \brief Parses a number as an unsigned long long
   * \param view the string view to parse
   * \return the number as unsigned long long
   * \warning Passed StringView must be pointing to a mutable buffer!
   */
  auto UnsignedLongLong(StringView view) noexcept -> UnsignedLL {
    UnsignedLL result{0};
    // strtoull accepts negative values!!
    // VECTOR NL AutosarC++17_10-A5.14.1: MD_JSON_AutosarC++17_10-A5.14.1_checked_access
    if ((!view.empty()) && (view[0] == '-')) {
      // Set end_ so that it will be reported as 'parsed unsuccessfully'
      // VECTOR NL AutosarC++17_10-A5.2.3: MD_JSON_AutosarC++17_10-A5.2.3_const_cast
      this->end_ = const_cast<char8_t*>(view.data());
    } else {
      internal::ResetErrno();
      // VECTOR NL AutosarC++17_10-A3.9.1: MD_JSON_AutosarC++17_10-A3.9.1_using
      result = std::strtoull(view.data(), &this->end_, static_cast<int>(this->base_));
    }
    return result;
  }

  /*!
   * \brief Parses a number as a double
   * \param view the string view to parse
   * \return the number as double
   */
  auto Double(StringView view) noexcept -> double {
    internal::ResetErrno();
    return std::strtod(view.data(), &this->end_);
  }

 private:
  /*!
   * \brief The Base to be used for this number
   */
  NumberBase base_;

  /*!
   * \brief Contains a pointer beyond the last parsed character or null if the parse did not work
   */
  char8_t* end_;
};
}  // namespace internal

/*!
 * \brief A representation of a parsed number
 *
 * \remark This parser differs from the JSON spec in that
 *         it does not support exponential notation for integers.
 * \pre This class assumes that all leading whitespace has been stripped
 *
 * \vpublic
 */
template <typename Ch = char8_t>
class Number final {
  /*!
   * \brief a string view specialized onto the current encoding
   * \todo Increase guarantees by switching to BasicCStringView
   *
   * \vprivate
   */
  using StringView = ara::core::StringView;

 public:
  /*!
   * \brief an unqualified optional type
   *
   * \vprivate
   */
  template <typename T>
  using optional = Optional<T>;

  /*!
   * \brief Constructor from a string view
   * \param view The string view to parse
   * \param base The base for which this number is valid
   *
   * \vprivate
   */
  explicit constexpr Number(StringView view, NumberBase base = NumberBase::kAutoDetect) noexcept
      : view_{view}, base_{base} {
    assert(!view.empty());
  }

  /*!
   * \brief Tries to convert the number to a type T
   * \returns either the converted number or a negative result
   * \error{JsonErrorDomain, JsonErrc::kInvalidNumber, Could not parse number}
   *
   * \vpublic
   */
  template <typename T>
  auto TryAs() const noexcept -> Result<T> {
    return MakeResult(this->As<T>(), JsonErrc::kInvalidNumber, "Could not convert number");
  }

  // VECTOR NC AutosarC++17_10-A15.5.3, AutosarC++17_10-A15.4.4: MD_JSON_noexcept_index, MD_JSON_noexcept_index
  /*!
   * \brief Tries to convert the number to a bool
   * \details Maps 1 to true and 0 to false
   * \returns either a bool or nothing
   *
   * \vpublic
   */
  template <typename Bool, std::enable_if_t<std::is_same<Bool, bool>::value>* = nullptr>
  auto As() const noexcept -> Optional<bool> {
    Optional<bool> opt;
    if (this->GetNumberOfChars() == 1) {
      switch (this->view_[0]) {
        case '1':
          opt.emplace(true);
          break;
        case '0':
          opt.emplace(false);
          break;
        default:
          // in case something different from 0 or 1 is included, there will be no conversion.
          break;
      }
    }
    return opt;
  }

  /*!
   * \brief Tries to convert the number to a signed number
   * \returns either the converted number or nothing
   *
   * \vpublic
   */
  template <typename Integer,
            std::enable_if_t<internal::IsInt<Integer>::value && std::is_signed<Integer>::value>* = nullptr>
  auto As() const noexcept -> Optional<Integer> {
    internal::Parser parser{this->base_};
    internal::SignedLL result{parser.LongLong(this->view_)};

    return this->ExtractIfSuccessful<Integer>(result, parser.End());
  }

  /*!
   * \brief Tries to convert the number to a signed number
   * \returns either the converted number or nothing
   *
   * \vpublic
   */
  template <typename Integer,
            std::enable_if_t<internal::IsInt<Integer>::value && !std::is_signed<Integer>::value>* = nullptr>
  auto As() const noexcept -> Optional<Integer> {
    internal::Parser parser{this->base_};
    internal::UnsignedLL result{parser.UnsignedLongLong(this->view_)};

    return this->ExtractIfSuccessful<Integer>(result, parser.End());
  }

  /*!
   * \brief Tries to convert the number to a double
   * \returns either the converted number or nothing
   *
   * \vpublic
   */
  template <typename Float, std::enable_if_t<std::is_floating_point<Float>::value>* = nullptr>
  auto As() const noexcept -> Optional<Float> {
    internal::Parser parser{this->base_};
    Optional<Float> result;

    if ((this->base_ == NumberBase::kDecimal) || (this->base_ == NumberBase::kZeroOnly)) {
      double const conversion_result{parser.Double(this->view_)};

      result = this->ExtractIfSuccessful<Float>(conversion_result, parser.End());
    }
    return result;
  }

  /*!
   * \brief Tries to convert the number to a vac::language::byte
   * \returns either the converted number or nothing
   */
  template <typename Byte, std::enable_if_t<std::is_same<vac::language::byte, Byte>::value>* = nullptr>
  auto As() const noexcept -> optional<Byte> {
    optional<Byte> opt;

    optional<std::uint8_t> conversion_result{this->As<std::uint8_t>()};
    if (conversion_result.has_value()) {
      opt.emplace(static_cast<Byte>(*conversion_result));
    }

    return opt;
  }

  /*!
   * \brief Convert the number using a user defined parser
   * \tparam Fn The function parser to convert string to a custom type
   * \param parser the parser function to convert the string view into a number
   * \returns the parsed number
   * \attention Fn must not throw an exception
   *
   * \vpublic
   */
  template <typename Fn>
  auto Convert(Fn&& parser) const noexcept -> decltype(parser(std::declval<StringView>())) {
    return parser(this->view_);
  }

 private:
  /*!
   * \brief Calculates the numbers of elements that have been parsed
   * \param end the pointer beyond the last parsed char
   * \returns the number of elements that have been parsed
   */
  auto NumParsedChars(char8_t const* end) const noexcept -> std::size_t {
    return static_cast<std::size_t>(std::distance(this->view_.data(), end));
  }

  /*!
   * \brief Gets the number of characters
   * \returns the number of characters
   */
  auto GetNumberOfChars() const noexcept -> std::size_t { return this->view_.size(); }

  /*!
   * \brief Casts the passed number to a smaller number type if still valid
   * \param num the source value
   */
  template <typename TargetType, typename SourceType>
  static auto Cast(SourceType num) noexcept -> Optional<TargetType> {
    Optional<TargetType> opt;
    // VECTOR NL Compiler-#186: MD_JSON_Compiler-#186-platform-variant
    if ((std::numeric_limits<TargetType>::max() >= num) && (std::numeric_limits<TargetType>::lowest() <= num)) {
      opt.emplace(TargetType(num));
    }
    return opt;
  }

  /*!
   * \brief Checks if the parse has been successful
   * \param end the end-pointer that is used to verify the parse
   */
  auto ParseSuccessful(char8_t const* end) const noexcept -> bool {
    bool successful{internal::GetErrno() != ERANGE};
    if (successful) {
      std::size_t const expected{this->GetNumberOfChars()};
      std::size_t const actual{this->NumParsedChars(end)};
      if (expected != actual) {
        successful = false;
      }
    }
    return successful;
  }

  /*!
   * \brief Casts the passed number to a smaller number type if still valid
   * \param num the source value
   * \param end the end iterator of the buffer
   */
  template <typename TargetType, typename SourceType>
  auto ExtractIfSuccessful(SourceType num, char8_t const* end) const noexcept -> Optional<TargetType> {
    Optional<TargetType> opt;
    if (this->ParseSuccessful(end)) {
      opt = Number::Cast<TargetType>(num);
    }
    return opt;
  }

  /*!
   * \brief The stored view onto the buffer
   */
  StringView view_;

  /*!
   * \brief The recognized base of the number
   */
  NumberBase base_;
};

/*!
 * \brief Typedef for Number.
 */
using JsonNumber = Number<>;
}  // namespace util
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_UTIL_NUMBER_H_
