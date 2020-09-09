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
/*!        \file  ara/core/string.h
 *        \brief  SWS core type ara::core::String.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_STRING_H_
#define LIB_VAC_INCLUDE_ARA_CORE_STRING_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <string>
#include <utility>

#include "ara/core/string_view.h"
#include "vac/memory/generated_memory_config.h"
#include "vac/memory/phase_managed_allocator.h"
#include "vac/memory/three_phase_allocator.h"

namespace ara {
namespace core {

/*!
 * \brief Basic_string type, as a base for the String implementation.
 */
template <typename character_type, class Traits = std::char_traits<character_type>,
          class Allocator = std::allocator<character_type>>
using basic_string = std::basic_string<character_type, Traits, Allocator>;

/*!
 * \brief Typedef for a string of arbitrary character types but fixed to the three phase allocator.
 * \vprivate
 */
template <typename character_type>
using basic_three_phase_string =
    basic_string<character_type, std::char_traits<character_type>, vac::memory::ThreePhaseAllocator<character_type>>;

/*!
 * \brief Typedef for a string of arbitrary character types but fixed to the PhaseManagedAllocator.
 * \vprivate
 */
template <typename character_type>
using phase_managed_string =
    basic_string<character_type, std::char_traits<character_type>, vac::memory::PhaseManagedAllocator<character_type>>;

/* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
/*!
 * \brief ThreePhaseAllocator-Managed equivalent of std::string.
 * \vprivate
 */
using SafeString = basic_three_phase_string<char>;

/* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
/*!
 * \brief Equivalent of std::string, that is phase-managed if deterministic mode is enabled.
 * \vprivate
 */
using ManagedString = phase_managed_string<char>;

/*!
 * \brief   Class that represents a character string.
 * \details Derives from std::string and includes extra Autosar-defined functionality.
 * \trace   CREQ-208384
 * \vpublic
 */
class String : public ManagedString {
 public:
  using ManagedString::ManagedString;

  /* VECTOR Next Construct AutosarC++17_10-A13.5.2: MD_VAC_A13.5.2_userDefinedConversionOperatorCStringView */
  /*!
   * \brief Converts the string to a StringView.
   */
  operator StringView() const noexcept { return StringView{data(), size()}; }

  /*!
   * \brief Default constructor.
   * \vpublic
   */
  String() = default;

  /*!
   * \brief Constructs a String from a StringViev.
   * \param sv StringView to construct from.
   * \vpublic
   */
  explicit String(StringView sv) : ManagedString(sv.data(), sv.size()) {}

  /*!
   * \brief  Constructs a String from a container convertible to a StringView.
   * \tparam T Container type that is convertible to a StringView.
   * \param  t Reference to a container.
   * \param  pos Start position for reading the container.
   * \param  n Number of characters to read.
   * \vpublic
   */
  template <typename T>
  String(T const& t, size_type pos, size_type n) : String(StringView{t}.substr(pos, n)) {}

  using ManagedString::operator=;
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /* VECTOR Next Construct AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_refQualifier */
  /*!
   * \brief  Assigns the contents of a StringView to the String.
   * \param  sv StringView to read from.
   * \return Reference to the modified string.
   * \vpublic
   */
  String& operator=(StringView sv) & { return assign(sv); }

  using ManagedString::assign;
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Assigns the contents of a StringView to the String.
   * \param  sv StringView to read from.
   * \return Reference to the modified string.
   * \vpublic
   */
  String& assign(StringView sv) {
    assign(sv.data(), sv.size());
    return *this;
  }

  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /* VECTOR Next Construct AutosarC++17_10-A8.5.2: MD_VAC_A8.5.2_provideExplicitInitializer */
  /*!
   * \brief  Assigns the contents of a container to the String.
   * \tparam T Container type that is convertible to a StringView.
   * \param  t Reference to a container.
   * \param  pos Start position for reading the container.
   * \param  n Number of character to read, npos to read until the end.
   * \return Reference to the modified string.
   * \vpublic
   */
  template <typename T>
  String& assign(T const& t, size_type pos, size_type n = StringView::npos) {
    StringView sv(t.data(), t.size());
    sv = sv.substr(pos, n);
    ManagedString::assign(sv.data(), 0, sv.size());
    return *this;
  }

  using ManagedString::operator+=;
  /* VECTOR Next Construct AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_refQualifier */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /* VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator */
  /*!
   * \brief  Appends the contents of a StringView to the String.
   * \param  sv StringView to append from.
   * \return Reference to the modified string.
   * \vpublic
   */
  String& operator+=(StringView sv) { return append(sv); }

  using ManagedString::append;
  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Appends the contents of a StringView to the String.
   * \param  sv StringView to append from.
   * \return Reference to the modified string.
   * \vpublic
   */
  String& append(StringView sv) {
    append(sv.data(), sv.size());
    return *this;
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /* VECTOR Next Construct AutosarC++17_10-A8.5.2: MD_VAC_A8.5.2_provideExplicitInitializer */
  /*!
   * \brief  Appends the contents of a container to the String.
   * \tparam T Container type that is convertible to a StringView.
   * \param  t Reference to a container.
   * \param  pos Start position for reading the container.
   * \param  n Number of character to read, npos to read until the end.
   * \return Reference to the modified string.
   * \vpublic
   */
  template <typename T>
  String& append(T const& t, size_type pos, size_type n = StringView::npos) {
    StringView sv(t.data(), t.size());
    sv = sv.substr(pos, n);
    if (sv.size() > 0) {
      append(sv);
    }
    return *this;
  }

  using ManagedString::insert;
  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Inserts the contents of a StringView into the String.
   * \param  pos Position in the String to insert to.
   * \param  sv StringView to insert from.
   * \return Reference to the modified string.
   * \vpublic
   */
  String& insert(size_type pos, StringView sv) {
    if (sv.size() > 0) {
      insert(pos, sv.data(), sv.size());
    }
    return *this;
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Inserts the contents of a container into the String.
   * \tparam T Container type that is convertible to a StringView.
   * \param  pos1 Position in the String to insert to.
   * \param  t Reference to a container.
   * \param  pos2 Position to start reading the container from.
   * \param  n Number of characters to read from the container, npos to read until the end.
   * \return Reference to the modified string.
   * \vpublic
   */
  template <typename T>
  String& insert(size_type pos1, T const& t, size_type pos2, size_type n = StringView::npos) {
    return insert(pos1, StringView{t.data(), t.size()}.substr(pos2, n));
  }

  using ManagedString::replace;
  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Replaces part of the String with the contents of a StringView.
   * \param  pos1 Position in the String to replace from.
   * \param  n1 Number of characters to replace.
   * \param  sv StringView to read from.
   * \return Reference to the modified string.
   * \vpublic
   */
  String& replace(size_type pos1, size_type n1, StringView sv) {
    ManagedString::replace(pos1, n1, sv.data(), sv.size());
    return *this;
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /* VECTOR Next Construct AutosarC++17_10-A8.5.2: MD_VAC_A8.5.2_provideExplicitInitializer */
  /*!
   * \brief  Replaces part of the String with the contents of a container.
   * \tparam T Container type that is convertible to a StringView.
   * \param  pos1 Position in the String to replace from.
   * \param  n1 Number of characters to replace.
   * \param  t Reference to a container.
   * \param  pos2 Position to start reading the container from.
   * \param  n2 Of characters to read from the container, npos to read until the end.
   * \return Reference to the modified string.
   * \vpublic
   */
  template <typename T>
  String& replace(size_type pos1, size_type n1, T const& t, size_type pos2, size_type n2 = StringView::npos) {
    StringView sv(t.data(), t.size());
    sv = sv.substr(pos2, n2);
    return replace(pos1, n1, sv);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Replaces an iterator-bounded part of the String with the contents of a StringView.
   * \param  i1 Iterator to the start of the replacement range (inclusive).
   * \param  i2 Iterator to the end of the replacement range (exclusive).
   * \param  sv StringView to read from.
   * \return Reference to the modified string.
   * \vpublic
   */
  String& replace(const_iterator i1, const_iterator i2, StringView sv) {
    // To use this workaround is to adapt to toolchains implementing erroneous std::string::replace signature since it
    // does not comply with C++11
    replace(static_cast<size_t>(i1 - begin()), static_cast<size_t>(i2 - i1), sv.data(), sv.size());
    return *this;
  }

  using ManagedString::find;
  using ManagedString::find_first_not_of;
  using ManagedString::find_first_of;
  using ManagedString::find_last_not_of;
  using ManagedString::find_last_of;
  using ManagedString::rfind;

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Finds the first substring equal to a StringView.
   * \param  sv StringView with data to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the first character of the found substring, or npos if not found.
   * \vpublic
   */
  size_type find(StringView sv, size_type pos = 0) const noexcept { return StringView(*this).find(sv, pos); }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Finds the last substring equal to a StringView.
   * \param  sv StringView with data to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the first character of the found substring, or npos if not found.
   * \vpublic
   */
  size_type rfind(StringView sv, size_type pos = StringView::npos) const noexcept {
    return StringView(*this).rfind(sv, pos);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Finds the first character equal to one of the characters in a StringView.
   * \param  sv StringView with characters to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the found character, of npos if not found.
   * \vpublic
   */
  size_type find_first_of(StringView sv, size_type pos = 0) const noexcept {
    return StringView(*this).find_first_of(sv, pos);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Finds the first character not equal to any of the characters in a StringView.
   * \param  sv StringView with characters to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the found character, of npos if not found.
   * \vpublic
   */
  size_type find_first_not_of(StringView sv, size_type pos = 0) const noexcept {
    return StringView(*this).find_first_not_of(sv, pos);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Finds the last character equal to one of the characters in a StringView.
   * \param  sv StringView with characters to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the found character, of npos if not found.
   * \vpublic
   */
  size_type find_last_of(StringView sv, size_type pos = StringView::npos) const noexcept {
    return StringView(*this).find_last_of(sv, pos);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief  Finds the last character not equal to any of the characters in a StringView.
   * \param  sv StringView with characters to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the found character, of npos if not found.
   * \vpublic
   */
  size_type find_last_not_of(StringView sv, size_type pos = StringView::npos) const noexcept {
    return find_last_not_of(String{sv}, pos);
  }

  using ManagedString::compare;

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Compares the String to a StringView lexicographically.
   * \param  sv StringView to compare to.
   * \return Negative value if this is lexicographically before the sv parameter, positive value if
   *         this is after the sv parameter, and 0 if this string and sv are equivalent.
   * \vpublic
   */
  int compare(StringView sv) const noexcept { return StringView(*this).compare(sv); }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Compares the substring of this String to a StringView lexicographically.
   * \param  pos1 Starting position of the substring.
   * \param  n1 Number of characters in the substring.
   * \param  sv StringView to compare to.
   * \return Negative value if the substring is lexicographically before the sv parameter, positive value if
   *         the substring is after the sv parameter, and 0 if they are equivalent.
   * \vpublic
   */
  int compare(size_type pos1, size_type n1, StringView sv) const noexcept {
    return StringView(*this).substr(pos1, n1).compare(sv);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec */
  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /* VECTOR Next Construct AutosarC++17_10-A8.5.2: MD_VAC_A8.5.2_provideExplicitInitializer */
  /*!
   * \brief  Compares the substring of this String to a substring of a container lexicographically.
   * \tparam T Container type that is convertible to a StringView.
   * \param  pos1 Starting position of the substring.
   * \param  n1 Number of characters in the substring.
   * \param  t Reference to a container.
   * \param  pos2 Starting position in the container.
   * \param  n2 Number of characters to read from the container.
   * \return Negative value if the substring is lexicographically before the container, positive value if
   *         the substring is after the container, and 0 if they are equivalent.
   * \vpublic
   */
  template <typename T>
  auto compare(size_type pos1, size_type n1, T const& t, size_type pos2, size_type n2 = StringView::npos) const noexcept
      -> int {
    StringView sv(StringView(t.data(), t.size()).substr(pos2, n2));
    return StringView(*this).substr(pos1, n1).compare(sv);
  }
};

/*!
 * \brief Swaps two Strings.
 * \param lhs Reference to the first string.
 * \param rhs Reference to the second string.
 * \vpublic
 */
inline void swap(String& lhs, String& rhs) noexcept { std::swap(lhs, rhs); }
}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_STRING_H_
