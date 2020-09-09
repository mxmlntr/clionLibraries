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
/*!        \file  c_string_view.h
 *        \brief  Implements a string_view object that is guaranteed to be null-terminated.
 *
 *      \details  Decays into a regular string_view for methods like substr.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_C_STRING_VIEW_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_C_STRING_VIEW_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <cstring>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "ara/core/string_view.h"
#include "ara/core/utility.h"
#include "vac/language/cpp17_backport.h"
#include "vac/language/throw_or_terminate.h"
#include "vac/memory/optional.h"

namespace vac {
namespace container {

/*!
 * \brief Basic_string_view Alias for ara::core::basic_string_view.
 */
template <typename T>
using basic_string_view = ara::core::basic_string_view<T>;

template <typename T>
class BasicCStringView;

/* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
/*!
 * \brief CStringView is a constant assortment of characters.
 * \trace CREQ-165660
 */
using CStringView = BasicCStringView<char>;

/* VECTOR Next Construct AutosarC++17_10-A2.14.3: MD_VAC_A2.14.3_wchar_t */
/*!
 * \brief WideCStringView is a constant assortment of wide characters.
 * \trace CREQ-165660
 */
using WideCStringView = BasicCStringView<wchar_t>;

/*!
 * \brief U16CStringView is a constant assortment of UCS-2 characters.
 * \trace CREQ-165660
 */
using U16CStringView = BasicCStringView<char16_t>;

/*!
 * \brief U32CStringView is a constant assortment of UCS-4 characters.
 * \trace CREQ-165660
 */
using U32CStringView = BasicCStringView<char32_t>;

/*!
 * \brief Represents a const view on some string buffer that is guaranteed to be null terminated.
 *        Does not own the string buffer.
 * \trace CREQ-165659
 */
template <typename Char>
class BasicCStringView final {
 public:
  /*!
   * \brief Typedef for the internal storage type.
   */
  using storage_t = basic_string_view<Char>;

  /*!
   * \brief Type of contained element.
   */
  using value_type = typename storage_t::value_type;

  /*!
   * \brief Type of the size field.
   */
  using size_type = typename storage_t::size_type;

  /*!
   * \brief Type of the differences between indices.
   */
  using difference_type = typename storage_t::difference_type;

  /*!
   * \brief Const reference type for contained elements.
   */
  using const_reference = typename storage_t::const_reference;

  /*!
   * \brief Const pointer type for contained elements.
   */
  using const_pointer = typename storage_t::const_pointer;

  /*!
   * \brief Const iterator for the BasicCStringView.
   */
  using const_iterator = typename storage_t::const_iterator;

  /*!
   * \brief Const reversed iterator for the BasicCStringView.
   */
  using const_reverse_iterator = typename storage_t::const_reverse_iterator;

  /*!
   * \brief       Construct a terminated_string_view from a raw char*.
   * \param       data Contains the string of characters.
   * \param       length The length of the string including \0.
   * \pre         The contained data must be a valid c-string.
   * \throws      std::invalid_argument If the passed container is not null terminated or a null pointer.
   * \internal
   *              - #10 Check that the string fits a \0 character.
   *              - #20 Check that the data pointer is valid.
   *              - #30 Check that only the last character is \0
   * \endinternal
   */
  BasicCStringView(const_pointer data, size_type length) : BasicCStringView(ara::core::in_place, data, (length - 1)) {
    /* #20 Check that the data pointer is valid */
    if (data == nullptr) {
      vac::language::ThrowOrTerminate<std::invalid_argument>("BasicCStringView: No nullptr allowed");
    }
    /* #10 Check that the string fits a \0 character */
    if (length == 0) {
      vac::language::ThrowOrTerminate<std::invalid_argument>(
          "BasicCStringView: Empty Strings cannot be zero terminated");
    }
    /* #30 Check that only the last character is \0 */
    if (this->delegate_.find('\0') != (length - 1)) {
      vac::language::ThrowOrTerminate<std::invalid_argument>("BasicCStringView: String is not null terminated");
    }
  }

  /*!
   * \brief   Templated constructor to convert from arbitrary containers.
   * \param   container A container providing contiguous data, ContainerType::data() and ContainerType::size().
   * \pre     The contained data must be a valid null-terminated c-string.
   * \details Is constexpr starting with C++14.
   * \throws  std::invalid_argument If the passed container is not null terminated.
   */
  template <typename ContainerType, typename DataType = decltype(std::declval<ContainerType const>().data()),
            typename = typename std::enable_if<std::is_same<DataType, const_pointer>::value>::type>
  explicit BasicCStringView(ContainerType const& container) noexcept(false)
      : BasicCStringView(container.data(), container.size()) {}

  /* VECTOR Next Construct AutosarC++17_10-A18.1.1: MD_VAC_A18.1.1_cStyleArraysShouldNotBeUsed */
  /*!
   * \brief  Creates a view of a string literal.
   * \tparam N Size of the char array, including null terminator.
   * \param  array C string literal.
   */
  template <std::size_t N>
  explicit BasicCStringView(Char const (&array)[N]) noexcept(false) : BasicCStringView(&array[0], N) {}

  /*!
   * \brief Explicitly disallow construction from only a raw pointer. This deletion avoids constructing from an
   *        implicitly converted temporary std::string. Uses template arguments to avoid ambiguity towards array type.
   */
  template <typename T,
            typename = std::enable_if_t<(std::is_same<T, Char const*>::value) || (std::is_same<T, Char*>::value)>>
  explicit BasicCStringView(T ptr) = delete;

  /*!
   * \brief   Constructor to convert from std::string.
   * \param   std_string An std::string to construct the CStringView from.
   * \details Is constexpr starting with C++14. This constructor is used because std::string violates the assumption of
   *          the templated constructor. For std::string, size() does not include the null terminator byte.
   * \throws  std::invalid_argument If the passed container is not null terminated.
   */
  explicit BasicCStringView(std::string const& std_string)
      : BasicCStringView(std_string.data(), std_string.size() + 1) {}

  /*!
   * \brief  Swap the contents of two string views.
   * \param  that The string view to swap.
   * \return void.
   */
  auto swap(BasicCStringView& that) noexcept -> void { std::swap(this->delegate_, that.delegate_); }

  /*!
   * \brief Copy constructor.
   */
  constexpr BasicCStringView(BasicCStringView const&) = default;

  /*! \brief Copy assignment.*/
  auto operator=(BasicCStringView const&) & -> BasicCStringView& = default;

  /*!
   * \brief Move constructor.
   */
  constexpr BasicCStringView(BasicCStringView&&) noexcept = default;

  /*!
   * \brief Move assignment.
   */
  auto operator=(BasicCStringView&&) & noexcept -> BasicCStringView& = default;

  /*!
   * \brief Get the length of the string without \0 terminator.
   */
  constexpr auto size() const -> size_type { return this->delegate_.size() - 1; }

  /*!
   * \brief Get the length of the string without \0 terminator.
   */
  constexpr auto length() const -> size_type { return this->size(); }

  /*!
   * \brief Determine whether the string is empty.
   */
  constexpr auto empty() const -> bool { return this->size() == 0; }

  /*!
   * \brief Get the raw data.
   */
  constexpr auto data() const -> const_pointer { return this->delegate_.data(); }

  /*!
   * \brief Get the raw C string.
   */
  constexpr auto c_str() const -> const_pointer { return this->data(); }

  /*!
   * \brief  Access a character at a specific index.
   * \param  index The location in the contained string.
   * \return A reference to the character at the specified index.
   * \throws std::out_of_range If the index is out of bounds.
   */
  auto at(size_type index) const -> const_reference {
    // Extra check so we don't return the terminating null byte
    if (index >= this->size()) {
      vac::language::ThrowOrTerminate<std::out_of_range>("BasicCStringView::at: Index out of range");
    }
    return this->delegate_.at(index);
  }

  /*!
   * \brief  Access a character at a specific location.
   * \param  index The location in the contained string.
   * \return A reference to the character at the specified index.
   */
  auto operator[](size_type index) const -> const_reference { return this->at(index); }

  /*!
   * \brief Iterator to the start of the string.
   */
  constexpr auto cbegin() const -> const_iterator { return this->delegate_.cbegin(); }

  /*!
   * \brief Iterator past the end  of the string.
   */
  constexpr auto cend() const -> const_iterator { return &this->delegate_.at(this->size()); }

  /*!
   * \brief Iterator to the start of the string.
   */
  constexpr auto begin() const -> const_iterator { return this->cbegin(); }

  /*!
   * \brief Iterator past the end  of the string.
   */
  constexpr auto end() const -> const_iterator { return this->cend(); }

  /*!
   * \brief Reference to the first character.
   */
  auto front() const -> const_reference { return this->delegate_.front(); }

  /*!
   * \brief  Reference to the last character.
   * \return A reference to the last character.
   * \throws std::out_of_range If the index is out of bounds.
   */
  auto back() const -> const_reference {
    if (this->size() == 0) {
      vac::language::ThrowOrTerminate<std::out_of_range>("BasicCStringView:Cannot call back on empty view");
    }
    return this->delegate_.at(this->size() - 1);
  }

  /*!
   * \brief   Returns a view of the substring.
   * \details Returns a view of the substring [pos, pos + rcount),
   *          where rcount is the smaller of count and size() - pos.
   * \param   pos Position of the first character.
   * \param   count Requested length.
   * \return  The substring view; empty if pos>=size().
   */
  auto substr(size_type pos = 0, size_type count = basic_string_view<Char>::npos) const noexcept
      -> basic_string_view<Char> {
    return this->delegate_.substr(pos, count);
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.5.2: MD_VAC_A13.5.2_userDefinedConversionOperatorCStringView */
  /*!
   * \brief  Decays the C string view into a normal, non-zero terminated string view.
   * \return The decayed string_view without the trailing \0.
   */
  constexpr operator basic_string_view<Char>() const noexcept { return {this->data(), this->size()}; }

  /*!
   * \brief     Create a string view from a literal that is guaranteed to be null terminated.
   * \param     data Contains the string of characters.
   * \param     length The length of the string excluding \0.
   * \return    The constructed C String.
   * \details   Needed because of GCC bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61648
   *            Otherwise the user-defined literals could be declared as a friend instead.
   * \attention This function does no check if the string is really \0 terminated and shall only be used by _sv
   *            operator.
   */
  constexpr static auto FromLiteral(Char const* data, std::size_t length) noexcept -> BasicCStringView<Char> {
    return {ara::core::in_place, data, length};
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.5.3: MD_VAC_A15.5.3_exceptionViolatesFunctionsNoexeceptSpec */
  /*!
   * \brief  Create a null-terminated string view from a basic_string.
   * \param  basic_string Contains the string of characters.
   * \return The constructed C String.
   */
  template <typename Allocator>
  static auto FromString(std::basic_string<Char, std::char_traits<Char>, Allocator> const& basic_string) noexcept(false)
      -> BasicCStringView<Char> {
    return {ara::core::in_place, basic_string.c_str(), basic_string.size()};
  }

  /*!
   * \brief  Creates an owned string from the CStringView.
   * \param  alloc Allocator for string instantiation.
   * \return The created string.
   */
  template <typename Allocator = memory::PhaseManagedAllocator<Char>>
  auto ToString(Allocator const& alloc = Allocator()) const noexcept(false)
      -> std::basic_string<Char, std::char_traits<Char>, Allocator> {
    return {this->data(), this->size(), alloc};
  }

 private:
  /*!
   * \brief Construct a terminated_string_view from a string with length without checks.
   * \param data Contains the string of characters.
   * \param length The length of the string excluding \0.
   */
  constexpr BasicCStringView(ara::core::in_place_t, const_pointer data, size_type length) noexcept
      : delegate_(data, length + 1) {}

  /*!
   * \brief Delegate String View.
   */
  storage_t delegate_;
};

/* VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator */
/* VECTOR Next Construct AutosarC++17_10-A13.2.2: MD_VAC_A13.2.2_bitwiseOperatorShallReturnBasicValue */
/*!
 * \brief  Enable formatted output for BasicCStringView.
 * \tparam Char The type of character encoding.
 * \param  os The output stream to write in.
 * \param  sv The string_view to write.
 * \return The same reference to the output stream.
 */
template <typename Char>
auto operator<<(std::basic_ostream<Char>& os, BasicCStringView<Char> const& sv) noexcept(false)
    -> std::basic_ostream<Char>& {
  return os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
}

/*!
 * \brief  Checks if the contents of both string views are the same.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if both contents are the same.
 */
template <typename Char>
constexpr auto operator==(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept(false)
    -> bool {
  basic_string_view<Char> temp_lhs{lhs};
  basic_string_view<Char> temp_rhs{rhs};
  return temp_lhs == temp_rhs;
}

/*!
 * \brief  Checks if the contents of both string views are the same.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if both contents are the same.
 */
template <typename Char>
constexpr auto operator==(BasicCStringView<Char> const& lhs, basic_string_view<Char> const& rhs) noexcept(false)
    -> bool {
  return basic_string_view<Char>(lhs) == rhs;
}

/*!
 * \brief  Checks if the contents of both string views are not the same.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if contents are not the same.
 */
template <typename Char>
constexpr auto operator!=(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept(false)
    -> bool {
  return !(lhs == rhs);
}

/*!
 * \brief  Checks if the contents of both string views are not the same.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if contents are not the same.
 */
template <typename Char>
constexpr auto operator!=(BasicCStringView<Char> const& lhs, basic_string_view<Char> const& rhs) noexcept(false)
    -> bool {
  return !(basic_string_view<Char>(lhs) == rhs);
}

/*!
 * \brief  Checks if the contents of this string view is smaller than the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is smaller than the other.
 */
template <typename Char>
constexpr auto operator<(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept(false) -> bool {
  basic_string_view<Char> temp_lhs{lhs};
  basic_string_view<Char> temp_rhs{rhs};
  return temp_lhs < temp_rhs;
}

/*!
 * \brief  Checks if the contents of this string view is smaller than the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is smaller than the other.
 */
template <typename Char>
constexpr auto operator<(BasicCStringView<Char> const& lhs, basic_string_view<Char> const& rhs) noexcept(false)
    -> bool {
  return basic_string_view<Char>(lhs) < rhs;
}

/*!
 * \brief  Checks if the contents of this string view is smaller or equal to the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is smaller or equal to the other.
 */
template <typename Char>
constexpr auto operator<=(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept(false)
    -> bool {
  return !(rhs < lhs);
}

/*!
 * \brief  Checks if the contents of this string view is smaller or equal to the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is smaller or equal to the other.
 */
template <typename Char>
constexpr auto operator<=(BasicCStringView<Char> const& lhs, basic_string_view<Char> const& rhs) noexcept(false)
    -> bool {
  return basic_string_view<Char>(lhs) <= rhs;
}

/*!
 * \brief  Checks if the contents of this string view is larger than the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is larger than the other.
 */
template <typename Char>
constexpr auto operator>(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept(false) -> bool {
  return rhs < lhs;
}

/*!
 * \brief  Checks if the contents of this string view is larger than the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is larger than the other.
 */
template <typename Char>
constexpr auto operator>(BasicCStringView<Char> const& lhs, basic_string_view<Char> const& rhs) noexcept(false)
    -> bool {
  return basic_string_view<Char>(lhs) > rhs;
}

/*!
 * \brief  Checks if the contents of this string view is larger or equal to the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is larger or equal to the other.
 */
template <typename Char>
constexpr auto operator>=(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept(false)
    -> bool {
  return !(lhs < rhs);
}

/*!
 * \brief  Checks if the contents of this string view is larger or equal to the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is larger or equal to the other.
 */
template <typename Char>
constexpr auto operator>=(BasicCStringView<Char> const& lhs, basic_string_view<Char> const& rhs) noexcept(false)
    -> bool {
  return basic_string_view<Char>(lhs) >= rhs;
}

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_C_STRING_VIEW_H_
