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
/*!        \file  ara/core/string_view.h
 *        \brief  SWS core type ara::core::StringView.
 *
 *      \details  Based on ara::core::Span implementation. For \0 terminated string_view, use CStringView.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_STRING_VIEW_H_
#define LIB_VAC_INCLUDE_ARA_CORE_STRING_VIEW_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <cstring>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "ara/core/span.h"
#include "vac/memory/phase_managed_allocator.h"
#include "vac/testing/test_adapter.h"

namespace ara {
namespace core {

/*!
 * \brief   Basic_string_view describes an object that can refer to a constant contiguous sequence of char-like objects
 *          with the first element of the sequence at position zero.
 * \details Basic_string_view  holds only two members: a const pointer T and a size.
 * \tparam  T The type of character encoding.
 * \trace   CREQ-158602
 * \vpublic
 */
template <typename T>
class basic_string_view final : public ara::core::Span<T const> {
  /*!
   * \brief Internally used type alias for storage type.
   */
  using span_type = ara::core::Span<T const>;

 public:
  /*!
   * \brief Type of contained element.
   */
  using value_type = typename span_type::value_type;

  /*!
   * \brief Type of the size field.
   */
  using size_type = std::size_t;

  /*!
   * \brief Type of the differences between indices.
   */
  using difference_type = std::ptrdiff_t;

  /*!
   * \brief Reference type for contained elements.
   */
  using reference = typename span_type::reference;

  /*!
   * \brief Const reference type for contained elements.
   */
  using const_reference = typename span_type::reference;

  /*!
   * \brief Pointer type for contained elements.
   */
  using pointer = typename span_type::pointer;

  /*!
   * \brief Const pointer type for contained elements.
   */
  using const_pointer = typename span_type::pointer;

  /*!
   * \brief Iterator for the basic_string_view.
   */
  using iterator = typename span_type::iterator;

  /*!
   * \brief Const iterator for the basic_string_view.
   */
  using const_iterator = typename span_type::const_iterator;

  /*!
   * \brief Reverse iterator for the basic_string_view.
   */
  using reverse_iterator = typename span_type::reverse_iterator;

  /*!
   * \brief Const reversed iterator for the basic_string_view.
   */
  using const_reverse_iterator = typename span_type::const_reverse_iterator;

  /*!
   * \brief Basis trait class for char type.
   */
  using char_trait = std::char_traits<T>;

  /*!
   * \brief End of view indicator by the functions that expect a view index.
   */
  static constexpr std::size_t npos{std::size_t(-1)};

  /* VECTOR Disable AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Disable AutosarC++17_10-A12.1.6: MD_VAC_A12.1.6_useInheritingConstructor */
  /*!
   * \brief Default constructor for an empty basic_string_view.
   * \vpublic
   */
  constexpr basic_string_view() noexcept : span_type() {}
  /* VECTOR Enable AutosarC++17_10-A12.1.6: MD_VAC_A12.1.6_useInheritingConstructor */

  /*!
   * \brief Constructor with a const pointer and size.
   *        When given a nullptr, the size is reset to 0.
   * \param data_val Pointer to the raw array containing the data.
   * \param data_size Length of data in number of elements.
   * \vpublic
   */
  constexpr basic_string_view(const_pointer data_val, size_type data_size) noexcept
      : span_type(data_val, ((data_val == nullptr) ? 0 : data_size)) {
    // Inline-If in constructor to support constexpr in C++11.
  }

  /*!
   * \brief   Constructor with a const pointer.
   * \param   data_val Pointer to the raw array containing the data.
   * \details Is constexpr starting with C++17.
   * \vpublic
   */
  constexpr explicit basic_string_view(const_pointer data_val) noexcept
      : basic_string_view(data_val, ((data_val == nullptr) ? 0 : char_trait::length(data_val))) {}

  /* VECTOR Disable AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect */
  /*!
   * \brief   Templated constructor to convert from arbitrary containers.
   *          When given a nullptr, the size is reset to 0.
   * \param   t A container providing contiguous data, ContainerType::data() and ContainerType::size().
   * \details Is constexpr starting with C++14.
   */
  template <typename ContainerType,
            typename = typename std::enable_if<
                std::is_same<decltype(std::declval<ContainerType const>().data()), const_pointer>::value>::type>
  constexpr explicit basic_string_view(ContainerType const& t) noexcept(
      (noexcept(std::declval<ContainerType>().data())) && (noexcept(std::declval<ContainerType>().size())))
      : basic_string_view(t.data(), t.size()) {}
  /* VECTOR Enable AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect */

  /*!
   * \brief Copy constructor.
   * \vpublic
   */
  basic_string_view(basic_string_view const&) noexcept = default;

  /* VECTOR Disable AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_refQualifier */
  /* VECTOR Disable AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Copy assignment.
   * \return A reference to the assigned-to basic_string_view.
   * \vpublic
   */
  basic_string_view& operator=(basic_string_view const&) & noexcept = default;

  /*!
   * \brief Move constructor.
   * \vprivate
   */
  basic_string_view(basic_string_view&&) noexcept = default;

  /*!
   * \brief  Move assignment.
   * \return A reference to the assigned-to basic_string_view.
   * \vpublic
   */
  basic_string_view& operator=(basic_string_view&&) & noexcept = default;
  /* VECTOR Enable AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /* VECTOR Enable AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_refQualifier */

  /*!
   * \brief  Get the length of the contained string.
   * \return The size of the string.
   * \vpublic
   */
  constexpr size_type length() const noexcept { return this->size(); }

  /* VECTOR Disable AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  compare the contents of two string_view objects
   * \param  other the second string view to compare
   * \return a negative value if this view is less than the other character sequence,
   *         zero if the both character sequences are equal or
   *         a positive value if this view is greater than the other character sequence.
   * \vpublic
   */
  int compare(basic_string_view const& other) const noexcept {
    // #10 Compare the first N chars where N is the minimum of both view's sizes.
    std::size_t min_size{std::min(this->size(), other.size())};
    int result{char_trait::compare(this->begin(), other.begin(), min_size)};

    // #20 If the first characters are equal.
    if (result == 0) {
      // #30 But both views have different sizes.
      if (this->size() != other.size()) {
        // #40 Then the shorter one is the smaller one.
        result = 1;
        if (this->size() < other.size()) {
          result = -1;
        }
      }
    }
    return result;
  }
  /* VECTOR Enable AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */

  /*!
   * \brief  Returns a view of the substring.
   * \param  pos Position of the first character.
   * \param  count Requested length.
   * \return View of the substring.
   * \vpublic
   */
  basic_string_view substr(size_type pos = 0, size_type count = npos) const noexcept {
    basic_string_view str;
    // #10 Unless the start position is greater than the original view's size
    if (pos <= this->size()) {
      // #20 We calculate the number of elements in the new view
      size_type end{this->size() - pos};
      count = std::min(count, end);
      // #30 We create a new view of the subset
      str = basic_string_view(std::next(this->data(), static_cast<std::ptrdiff_t>(pos)), count);
    }
    return str;
  }

  /*!
   * \brief  Returns the position of found character.
   * \param  candidate Character to search for.
   * \param  pos Position of the first character.
   * \return Position of the first character of the found substring, or npos if no such substring is found.
   * \vpublic
   */
  size_type find(value_type candidate, size_type pos = 0) const noexcept {
    iterator result{std::find(std::next(this->begin(), static_cast<std::ptrdiff_t>(pos)), this->end(), candidate)};
    return this->IteratorPosToIndex(result);
  }

  /*!
   * \brief  Returns the position of found string.
   * \param  candidate Pointer to a character string to search for.
   * \param  pos Position of the first character.
   * \param  len Length of substring to search for.
   * \return Position of the first character of the found substring, or npos if no such substring is found.
   * \vpublic
   */
  size_type find(const_pointer candidate, size_type pos, size_type len) const noexcept {
    size_type ret_value{npos};
    if (candidate != nullptr) {
      ret_value = this->find(basic_string_view(candidate, len), pos);
    }
    return ret_value;
  }

  /*!
   * \brief  Returns the position of found character.
   * \param  candidate View to search for.
   * \param  pos Position at which to start the search.
   * \return Position of the first character of the found substring, or npos if no such substring is found.
   * \vpublic
   */
  size_type find(basic_string_view candidate, size_type pos = 0) const noexcept {
    size_type ret_value;
    if (this->size() < pos) {
      ret_value = npos;
    }
    iterator result{std::search(std::next(this->begin(), static_cast<std::ptrdiff_t>(pos)), this->end(),
                                candidate.begin(), candidate.end())};
    ret_value = this->IteratorPosToIndex(result);
    return ret_value;
  }

  /*!
   * \brief    Finds the last substring equal to the given character sequence.
   * \param    candidate View to search for.
   * \param    pos Position at which to start the search.
   * \return   Position of the first character of the found substring or npos if no such substring is found.
   * \internal
   *           - #10 Check that the substring is not longer than the string.
   *           - #20 Get a start position that is inside the views bounds.
   *           - #30 Given the view is empty, report a invalid position.
   *             - #40 Given the candidate view is empty, report the last position that is inside the specified range.
   *             - #50 Calculate the start position from the end.
   *             - #60 Search for the substring from right to left.
   *             - #70 Translate the iterator position to the wanted index.
   * \endinternal
   * \vpublic
   */
  size_type rfind(const basic_string_view candidate, const size_type pos = npos) const noexcept {
    size_type ret_value;
    // #10 Check that the substring is not longer than the string.
    if (candidate.size() > this->size()) {
      ret_value = npos;
    }
    // #20 Get a start position that is inside the views bounds.
    std::size_t const validated_pos{std::min(pos, this->size() - candidate.size())};
    // #30 Given the view is empty, report a invalid position.
    if (this->empty()) {
      ret_value = npos;
    } else if (candidate.empty()) {
      // #40 Given the candidate view is empty, report the last position that is inside the specified range.
      ret_value = validated_pos;
    } else {
      // #50 Otherwise Calculate the start position from the end.
      std::size_t const reversed_start_pos{validated_pos + candidate.size()};
      // #60 Search for the substring from right to left.
      const_reverse_iterator const reverse_begin{
          std::next(this->cbegin(), static_cast<std::ptrdiff_t>(reversed_start_pos))};
      const_reverse_iterator const reverse_end{this->crend()};
      const_reverse_iterator const candidate_reverse_begin{candidate.crbegin()};
      const_reverse_iterator const candidate_reverse_end{candidate.crend()};
      const_reverse_iterator const result{
          std::search(reverse_begin, reverse_end, candidate_reverse_begin, candidate_reverse_end)};
      // #70 Translate the iterator position to the wanted index.
      ret_value = this->ReverseIteratorPosToIndex(result, candidate.size() - 1);
    }
    return ret_value;
  }

  /*!
   * \brief  Finds the last substring equal to the given character sequence.
   * \param  ch Pointer to a character string to search for.
   * \param  pos Position at which to start the search.
   * \param  count Length of substring to search for.
   * \return Position of the first character of the found substring or npos if no such substring is found.
   * \vpublic
   */
  size_type rfind(const_pointer ch, size_type pos, size_type count) const noexcept {
    return this->rfind(basic_string_view(ch, count), pos);
  }

  /*!
   * \brief  Finds the first character equal to any of the characters in the given character sequence.
   * \param  candidate View to search for.
   * \param  pos Position at which to start the search.
   * \return Position of the first occurrence of any character of the substring, or npos if no such character is found.
   * \vpublic
   */
  size_type find_first_of(basic_string_view candidate, size_type pos = 0) const noexcept {
    size_type ret_value{npos};
    if (this->size() >= pos) {
      iterator result{/* VECTOR Next Line AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_arrayIndexingOnlyAllowedForArrays */
                      std::find_first_of(std::next(this->begin(), static_cast<std::ptrdiff_t>(pos)), this->end(),
                                         candidate.begin(), candidate.end())};
      ret_value = this->IteratorPosToIndex(result);
    }
    return ret_value;
  }

  /*!
   * \brief  Finds the first character equal to any of the characters in the given character sequence.
   * \param  candidate Character to search for.
   * \param  pos Position at which to start the search.
   * \return Position of the first occurrence of any character of the substring, or npos if no such character is found.
   * \vpublic
   */
  size_type find_first_of(value_type candidate, size_type pos = 0) const noexcept { return find(candidate, pos); }

  /*!
   * \brief  Finds the first character equal to any of the characters in the given character sequence.
   * \param  candidate Character to search for.
   * \param  pos Position at which to start the search.
   * \param  count Length of the string of characters to search for.
   * \return Position of the first occurrence of any character of the substring, or npos if no such character is found.
   * \vpublic
   */
  size_type find_first_of(const_pointer candidate, size_type pos, size_type count) const noexcept {
    size_type ret_value{npos};
    if (candidate != nullptr) {
      ret_value = this->find_first_of(basic_string_view(candidate, count), pos);
    }
    return ret_value;
  }

  /*!
   * \brief  Finds the first character not equal to any of the characters in the given character sequence.
   * \param  candidate View to search for.
   * \param  pos Position at which to start the search.
   * \return Position of the first occurrence of any character not equal to any character of the substring, or npos if
   *         no such character is found.
   * \vpublic
   */
  size_type find_first_not_of(basic_string_view candidate, size_type pos = 0) const noexcept {
    size_type retval{npos};
    for (; pos < this->size(); ++pos) {
      size_type p{candidate.find((*this)[pos])};
      if (p == npos) {
        retval = pos;
        break;
      }
    }
    return retval;
  }

  /*!
   * \brief  Finds the first character not equal to the given character.
   * \param  candidate Character to search for.
   * \param  pos Position at which to start the search.
   * \return Position of the first occurrence not equal to the given character, or npos if no such character is found.
   * \vpublic
   */
  size_type find_first_not_of(value_type candidate, size_type pos = 0) const noexcept {
    iterator result{std::find_if_not(std::next(this->begin(), pos), this->end(),
                                     [candidate](value_type ch) { return ch == candidate; })};
    return this->IteratorPosToIndex(result);
  }

  /*!
   * \brief  Finds the first character not equal to any of the characters in the given character sequence.
   * \param  candidate Character to search for.
   * \param  pos Position at which to start the search.
   * \param  count Length of the string of characters to search for.
   * \return Position of the first occurrence of any character not equal to any character of the substring, or npos if
   *         no such character is found.
   * \vpublic
   */
  size_type find_first_not_of(const_pointer candidate, size_type pos, size_type count) const noexcept {
    size_type ret_value{npos};
    if (candidate != nullptr) {
      ret_value = find_first_not_of(basic_string_view{candidate, count}, pos);
    }
    return ret_value;
  }

  /*!
   * \brief  Finds the last character equal to any of the characters in the given character sequence.
   * \param  candidate Pointer to a string of characters to search for.
   * \param  pos Position in this string_view at which to start the search.
   * \param  count Length of the string of characters to search for.
   * \return Position of the last occurrence of any character of the substring, or npos if no such character is found.
   * \vpublic
   */
  size_type find_last_of(const_pointer candidate, size_type pos, size_type count) const noexcept {
    return find_last_of(basic_string_view{candidate, count}, pos);
  }

  /*!
   * \brief  Finds the last character equal to any of the characters in the given character sequence.
   * \param  candidate View to search for.
   * \param  pos Position this string_view at which to start the search.
   * \return Position of the last occurrence of any character of the substring, or npos if no such character is found.
   * \vpublic
   */
  size_type find_last_of(basic_string_view candidate, size_type pos = npos) const noexcept {
    size_type ret_value{npos};
    if (this->size_ > 0) {
      size_type const max_pos{this->size_ - 1u};
      pos = std::min(pos, this->size_ - 1u);  // pos must point to a valid character
      size_type self_check_position{0u};
      bool done{false};
      // iterate this data
      /* VECTOR Next Line AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness */
      const_reverse_iterator it{std::next(this->crbegin(), static_cast<difference_type>(max_pos - pos))};
      for (; it != this->crend(); ++it) {
        // iterate input data
        for (const_iterator cand_it{candidate.cbegin()}; cand_it != candidate.cend(); ++cand_it) {
          if (*it == *cand_it) {
            ret_value = pos - self_check_position;
            done = true;
            break;
          }
        }
        if (done) {
          break;
        }
        ++self_check_position;
      }
    }
    return ret_value;
  }

  /*!
   * \brief  Finds the last occurrence of a given character.
   * \param  candidate Character to search for.
   * \param  pos Position this string_view at which to start the search.
   * \return Position of the last occurrence of the character, or npos if no such character is found.
   * \vpublic
   */
  size_type find_last_of(value_type candidate, size_type pos = npos) const noexcept {
    return find_last_of(basic_string_view{&candidate, 1}, pos);
  }

  /*!
   * \brief  Finds the last character equal to any of the characters in the given character sequence.
   * \param  candidate Null-terminated sequence of characters to search for.
   * \param  pos Position this string_view at which to start the search.
   * \return Position of the last occurrence of any character of the substring, or npos if no such character is found.
   * \vpublic
   */
  size_type find_last_of(const_pointer candidate, size_type pos = npos) const noexcept {
    return find_last_of(basic_string_view{candidate, char_trait::length(candidate)}, pos);
  }

  /*!
   * \brief  Checks if the string_view contains a null terminating symbol.
   * \return Returns true if a null character is contained, false if not.
   * \vprivate
   */
  bool ContainsNullCharacter() const noexcept { return this->find_last_of('\0') != npos; }

  /*!
   * \brief  Checks if the string_view ends with a null terminating symbol.
   * \return Returns true if the view is terminated with a null character.
   * \vprivate
   */
  bool IsNullTerminated() const noexcept {
    bool ret_value{false};
    if (((this->data_) != nullptr) && (!this->empty())) {
      const_reference back{this->back()};
      ret_value = back == '\0';
    }
    return ret_value;
  }

  /*!
   * \brief Moves the end of the view back by n characters.
   * \param n Number of characters to remove from the end of the view.
   * \vpublic
   */
  void remove_suffix(size_type n) noexcept {
    if (n < this->size_) {
      this->size_ -= n;
    } else {
      this->size_ = 0;
    }
  }

  /*!
   * \brief Moves the start of the view forward by n characters.
   * \param n Number of characters to remove from the start of the view.
   * \vpublic
   */
  void remove_prefix(size_type n) noexcept {
    if (this->data_ != nullptr) {
      n = std::min(n, this->size_);
      std::advance(this->data_, n);
      this->size_ -= n;
    }
  }

  /*!
   * \brief Returns the first element.
   * \return The first element.
   * \vpublic
   */
  const_reference front() const { return at(0); }

  /*!
   * \brief  Returns the last element.
   * \return The last element.
   * \vpublic
   */
  const_reference back() const { return at(this->size() - 1); }

  /*!
   * \brief  Returns the character at specified index.
   * \param  i Index of the character
   * \return The character at specified index.
   * \vpublic
   */
  const_reference at(size_type i) const { return this->operator[](i); }

  /*!
   * \brief  Creates an owned string from the string_view.
   * \param  alloc Allocator for string instantiation.
   * \return The created string.
   * \vprivate
   */
  template <typename Allocator = vac::memory::PhaseManagedAllocator<T>>
  auto ToString(Allocator const& alloc = Allocator()) const -> std::basic_string<T, char_trait, Allocator> {
    return {this->data(), this->size(), alloc};
  }

  /*!
   * \brief Swap specialization.
   * \param other StringView to be swapped
   * \vpublic
   */
  void swap(basic_string_view& other) { std::swap(*this, other); }

 private:
  /*!
   * \brief  Finds the position in the view for a passed iterator.
   * \param  it The iterator to check.
   * \return The position of the iterator.
   */
  size_type IteratorPosToIndex(iterator it) const noexcept {
    size_type ret_value{npos};
    if (it != this->end()) {
      difference_type const diff{std::distance(this->begin(), it)};
      assert(diff >= 0);
      ret_value = static_cast<size_type>(diff);
    }
    return ret_value;
  }

  /*!
   * \brief    Finds the position in the view for a passed reverse iterator.
   * \param    reverse_it The reverse_iterator to convert to an index.
   * \param    offset The index is moved by offset.
   * \return   The position of the iterator.
   * \pre      Offset must be smaller than this->size() & this must not be empty.
   * \internal
   *           - #10 Given that the passed iterator points beyond the last element, return the invalid position
   *           - #20 Otherwise return the distance between the iterator position and the start.
   * \endinternal
   */
  size_type ReverseIteratorPosToIndex(const_reverse_iterator reverse_it, std::size_t offset = 0) const noexcept {
    size_type ret_value{npos};
    // #10 Given that the passed iterator points beyond the last element, return the invalid position.
    if (reverse_it != this->crend()) {
      // #20 Otherwise return the distance between the iterator position and the start.
      const_iterator forward_it{const_reverse_iterator{reverse_it}.base()};
      ret_value = static_cast<size_type>(std::distance(this->cbegin(), forward_it)) - (offset + 1);
    }
    return ret_value;
  }

  /*!
   * \brief Friend declaration for testing.
   */
  FRIEND_TEST(StringViewFixture, At);
};

namespace internal {

/*!
 *   \brief Identity type alias for basic_string_view
 *  \tparam T The type of character encoding
 * \vprivate
 */
template <typename T>
using BsvIdentity = std::decay_t<T>;

}  // namespace internal

/*!
 * \brief   Checks if the contents of two string views are the same
 * \tparam  T The type of character encoding
 * \param   lhs the first string view to compare
 * \param   rhs the second string view to compare
 * \returns true if both contents are the same
 * \vpublic
 */
template <typename T>
constexpr bool operator==(basic_string_view<T> lhs, basic_string_view<T> rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

/*!
 * \brief   Checks if the contents of a string view and a a string view implicitly convertible
 *          object are the same
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the object to compare
 * \returns true if both contents are the same
 * \vpublic
 */
template <typename T>
constexpr bool operator==(basic_string_view<T> lhs, internal::BsvIdentity<basic_string_view<T>> rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

/*!
 * \brief   Checks if the contents of a string view implicitly convertible object and a
 *          string view are the same
 * \tparam  T The type of character encoding
 * \param   lhs the object to compare
 * \param   rhs the string view to compare
 * \returns true if both contents are the same
 * \vpublic
 */
template <typename T>
constexpr bool operator==(internal::BsvIdentity<basic_string_view<T>> lhs, basic_string_view<T> rhs) noexcept {
  return rhs == lhs;
}

/*!
 * \brief   Checks if the contents of a string view and a pointer are the same
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the pointer to compare
 * \returns true if both contents are the same
 * \vpublic
 */
template <typename T>
constexpr bool operator==(basic_string_view<T> lhs, T const* rhs) noexcept {
  return lhs.compare(basic_string_view<T>(rhs)) == 0;
}

/*!
 * \brief   Checks if the contents of a string view and a pointer are the same
 * \tparam  T The type of character encoding
 * \param   lhs the pointe' to compare
 * \param   rhs the string view to compare
 * \returns true if both contents are the same
 * \vpublic
 */
template <typename T>
constexpr bool operator==(T const* lhs, basic_string_view<T> rhs) noexcept {
  return rhs == lhs;
}

/*!
 * \brief   Checks if the contents of two string views are not the same
 * \tparam  T The type of character encoding
 * \param   lhs the first string view to compare
 * \param   rhs the second string view to compare
 * \returns true if both contents are not the same
 * \vpublic
 */
template <typename T>
constexpr bool operator!=(basic_string_view<T> lhs, basic_string_view<T> rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief   Checks if the contents of a string view and a string view implicitly convertible
 *          object are not the same
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the object to compare
 * \returns true if both contents are not the same
 * \vpublic
 */
template <typename T>
constexpr bool operator!=(basic_string_view<T> lhs, internal::BsvIdentity<basic_string_view<T>> rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief   Checks if the contents of a string view implicitly convertible object and a string view
 *          are not the same
 * \tparam  T The type of character encoding
 * \param   lhs the object to compare
 * \param   rhs the string view to compare
 * \returns true if both contents are not the same
 * \vpublic
 */
template <typename T>
constexpr bool operator!=(internal::BsvIdentity<basic_string_view<T>> lhs, basic_string_view<T> rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief   Checks if the contents of a string view and a pointer are not the same
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the pointer to compare
 * \returns true if both contents are not the same
 * \vpublic
 */
template <typename T>
constexpr bool operator!=(basic_string_view<T> lhs, T const* rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief   Checks if the contents of a string view and a pointer are not the same
 * \tparam  T The type of character encoding
 * \param   lhs the pointer to compare
 * \param   rhs the string view to compare
 * \returns true if both contents are not the same
 * \vpublic
 */
template <typename T>
constexpr bool operator!=(T const* lhs, basic_string_view<T> rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief   Checks if the contents of the first string view is smaller than the second string view
 * \tparam  T The type of character encoding
 * \param   lhs the first string view to compare
 * \param   rhs the second string view to compare
 * \returns true if the contents of the first string view is smaller than the second string view
 * \vpublic
 */
template <typename T>
constexpr bool operator<(basic_string_view<T> lhs, basic_string_view<T> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

/*!
 * \brief   Checks if the contents of a string view is smaller than the contents of a string view
 *          implicitly convertible object
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the object to compare
 * \returns true if the contents of a string view is smaller than the contents of an object
 * \vpublic
 */
template <typename T>
constexpr bool operator<(basic_string_view<T> lhs, internal::BsvIdentity<basic_string_view<T>> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

/*!
 * \brief   Checks if the contents of a string view implicitly convertible object
 *          is smaller than the contents of a string view
 * \tparam  T The type of character encoding
 * \param   lhs the object view to compare
 * \param   rhs the string view to compare
 * \returns true if the contents of an object is smaller than the contents of a string view
 * \vpublic
 */
template <typename T>
constexpr bool operator<(internal::BsvIdentity<basic_string_view<T>> lhs, basic_string_view<T> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

/*!
 * \brief   Checks if the contents of a string view is smaller than the contents of a pointer
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the pointer to compare
 * \returns true if the contents of a string view is smaller than the contents of an object
 * \vpublic
 */
template <typename T>
constexpr bool operator<(basic_string_view<T> lhs, T const* rhs) noexcept {
  return lhs.compare(basic_string_view<T>(rhs)) < 0;
}

/*!
 * \brief   Checks if the contents of a pointer is smaller than the contents of a string view
 * \tparam  T The type of character encoding
 * \param   lhs the pointer to compare
 * \param   rhs the string view to compare
 * \returns true if the contents of a pointer is smaller than the contents of a string view
 * \vpublic
 */
template <typename T>
constexpr bool operator<(T const* lhs, basic_string_view<T> rhs) noexcept {
  return basic_string_view<T>(lhs).compare(rhs) < 0;
}

/*!
 * \brief   Checks if the contents of the first string view is greater than the second string view
 * \tparam  T The type of character encoding
 * \param   lhs the first string view to compare
 * \param   rhs the second string view to compare
 * \returns true if the contents of the first string view is greater than the second string view
 * \vpublic
 */
template <typename T>
constexpr bool operator>(basic_string_view<T> lhs, basic_string_view<T> rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief   Checks if the contents of a string view is greater than the contents of a string view
 *          implicitly convertible object
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the object to compare
 * \returns true if the contents of a string view is greater than the contents of an object
 * \vpublic
 */
template <typename T>
constexpr bool operator>(basic_string_view<T> lhs, internal::BsvIdentity<basic_string_view<T>> rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief   Checks if the contents of a string view implicitly convertible object
 *          is greater than the contents of a string view
 * \tparam  T The type of character encoding
 * \param   lhs the object view to compare
 * \param   rhs the string view to compare
 * \returns true if the contents of an object is greater than the contents of a string view
 * \vpublic
 */
template <typename T>
constexpr bool operator>(internal::BsvIdentity<basic_string_view<T>> lhs, basic_string_view<T> rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief   Checks if the contents of a string view is greater than the contents of a pointer
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the pointer to compare
 * \returns true if the contents of a string view is greater than the contents of an object
 * \vpublic
 */
template <typename T>
constexpr bool operator>(basic_string_view<T> lhs, T const* rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief   Checks if the contents of a pointer is greater than the contents of a string view
 * \tparam  T The type of character encoding
 * \param   lhs the pointer to compare
 * \param   rhs the string view to compare
 * \returns true if the contents of a pointer is greater than the contents of a string view
 * \vpublic
 */
template <typename T>
constexpr bool operator>(T const* lhs, basic_string_view<T> rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief   Checks if the contents of the first string view is smaller equal than the second string view
 * \tparam  T The type of character encoding
 * \param   lhs the first string view to compare
 * \param   rhs the second string view to compare
 * \returns true if the contents of the first string view is smaller equal than the second string view
 * \vpublic
 */
template <typename T>
constexpr bool operator<=(basic_string_view<T> lhs, basic_string_view<T> rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief   Checks if the contents of a string view is smaller equal than the contents of a
 *          string view implicitly convertible object
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the object to compare
 * \returns true if the contents of a string view is smaller equal than the contents of an object
 * \vpublic
 */
template <typename T>
constexpr bool operator<=(basic_string_view<T> lhs, internal::BsvIdentity<basic_string_view<T>> rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief   Checks if the contents of a string view implicitly convertible object
 *          is smaller equal than the contents of a string view
 * \tparam  T The type of character encoding
 * \param   lhs the object view to compare
 * \param   rhs the string view to compare
 * \returns true if the contents of an object is smaller equal than the contents of a string view
 * \vpublic
 */
template <typename T>
constexpr bool operator<=(internal::BsvIdentity<basic_string_view<T>> lhs, basic_string_view<T> rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief   Checks if the contents of a string view is smaller equal than the contents of a pointer
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the pointer to compare
 * \returns true if the contents of a string view is smaller equal than the contents of an object
 * \vpublic
 */
template <typename T>
constexpr bool operator<=(basic_string_view<T> lhs, T const* rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief   Checks if the contents of a pointer is smaller equal than the contents of a string view
 * \tparam  T The type of character encoding
 * \param   lhs the pointer to compare
 * \param   rhs the string view to compare
 * \returns true if the contents of a pointer is smaller equal than the contents of a string view
 * \vpublic
 */
template <typename T>
constexpr bool operator<=(T const* lhs, basic_string_view<T> rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief   Checks if the contents of the first string view is greater equal than the second string view
 * \tparam  T The type of character encoding
 * \param   lhs the first string view to compare
 * \param   rhs the second string view to compare
 * \returns true if the contents of the first string view is greater equal than the second string view
 * \vpublic
 */
template <typename T>
constexpr bool operator>=(basic_string_view<T> lhs, basic_string_view<T> rhs) noexcept {
  return !(lhs < rhs);
}

/*!
 * \brief   Checks if the contents of a string view is greater equal than the contents of a string view
 *          implicitly convertible object
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the object to compare
 * \returns true if the contents of a string view is greater equal than the contents of an object
 * \vpublic
 */
template <typename T>
constexpr bool operator>=(basic_string_view<T> lhs, internal::BsvIdentity<basic_string_view<T>> rhs) noexcept {
  return !(lhs < rhs);
}

/*!
 * \brief   Checks if the contents of a string view implicitly convertible object is greater equal
 *          than the contents of a string view
 * \tparam  T The type of character encoding
 * \param   lhs the object view to compare
 * \param   rhs the string view to compare
 * \returns true if the contents of an object is greater equal than the contents of a string view
 * \vpublic
 */
template <typename T>
constexpr bool operator>=(internal::BsvIdentity<basic_string_view<T>> lhs, basic_string_view<T> rhs) noexcept {
  return !(lhs < rhs);
}

/*!
 * \brief   Checks if the contents of a string view is greater equal than the contents of a pointer
 * \tparam  T The type of character encoding
 * \param   lhs the string view to compare
 * \param   rhs the pointer to compare
 * \returns true if the contents of a string view is greater equal than the contents of an object
 * \vpublic
 */
template <typename T>
constexpr bool operator>=(basic_string_view<T> lhs, T const* rhs) noexcept {
  return !(lhs < rhs);
}

/*!
 * \brief   Checks if the contents of a pointer is greater equal than the contents of a string view
 * \tparam  T The type of character encoding
 * \param   lhs the pointer to compare
 * \param   rhs the string view to compare
 * \returns true if the contents of a pointer is greater equal than the contents of a string view
 * \vpublic
 */
template <typename T>
constexpr bool operator>=(T const* lhs, basic_string_view<T> rhs) noexcept {
  return !(lhs < rhs);
}

template <typename T>
constexpr std::size_t basic_string_view<T>::npos;

/* VECTOR Disable AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
/*!
 * \brief StringView is a constant assortment of characters.
 * \vpublic
 */
using StringView = basic_string_view<char>;
/* VECTOR Enable AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */

/* VECTOR Disable AutosarC++17_10-A13.2.2: MD_VAC_A13.2.2_bitwiseOperatorShallReturnBasicValue */
/*!
 * \brief  Enable formatted output for string_view.
 * \tparam Char The type of character encoding.
 * \param  os The output stream to write in.
 * \param  sv The string_view to write.
 * \return The same reference to the output stream.
 * \vpublic
 */
template <typename Char>
auto operator<<(std::basic_ostream<Char>& os, basic_string_view<Char> const& sv) -> std::basic_ostream<Char>& {
  return os.write(sv.data(), sv.size());
}
/* VECTOR Enable AutosarC++17_10-A13.2.2: MD_VAC_A13.2.2_bitwiseOperatorShallReturnBasicValue */
/* VECTOR Enable AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */

}  // namespace core
}  // namespace ara

/* VECTOR Disable AutosarC++17_10-M16.0.1: MD_VAC_M16.0.1_includePrecededByPreprocessor */
/*!
 * \brief Need to include this here as some other modules using it through include by chance,
 *        and those modules were able to compile/build successfully before moving stringView
 *        from vac::container to ara::core.
 */
#include "vac/container/string_view.h"
/* VECTOR Enable AutosarC++17_10-M16.0.1: MD_VAC_M16.0.1_includePrecededByPreprocessor */

#endif  // LIB_VAC_INCLUDE_ARA_CORE_STRING_VIEW_H_
