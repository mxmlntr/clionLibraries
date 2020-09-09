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
/*!        \file  static_string_stream.h
 *        \brief  A static string stream stores a sequence of characters in contiguous memory.
 *
 *      \details  Header file of vac::container::StaticStringStream.
 *                Check the capacity in the storage, reset the storage and append data to the
 *                StaticStringStream.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_STRING_STREAM_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_STRING_STREAM_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <utility>

#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "vac/language/throw_or_terminate.h"

namespace vac {
namespace container {

/*!
 * \brief StringView Alias for ara::core::StringView.
 */
using StringView = ara::core::StringView;

/*!
 * \brief A static stringstream implementation.
 * \trace CREQ-158598
 */
class StaticStringStream final {
 private:
  /*!
   * \brief  Type of the underlying data.
   * \remark The underlying data type depends on PhaseManagedAllocator, i.e. allocation/deallocation phase check only if
   *         enabled in PhaseManagedAllocator.
   * \trace  CREQ-160857
   */
  using base_type = ara::core::ManagedString;

 public:
  /*!
   * \brief The size_type.
   */
  using size_type = base_type::size_type;

  /*!
   * \brief Constructor which reserves a specified amount of memory.
   * \param size The amount of memory to be reserved.
   * \trace CREQ-158592
   */
  explicit StaticStringStream(size_type size) : data_() { data_.reserve(size); }

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief Constructor.
   * \param data Pointer to the character string to store.
   */
  explicit StaticStringStream(const char* data) : data_(data) {}

  /*!
   * \brief Destructor.
   */
  ~StaticStringStream() = default;
  /*!
   * \brief Default copy constructor deleted.
   */
  StaticStringStream(StaticStringStream const&) = delete;
  /*!
   * \brief Default copy assignment operator deleted.
   */
  StaticStringStream& operator=(StaticStringStream const&) = delete;
  /*!
   * \brief Default move constructor deleted.
   */
  StaticStringStream(StaticStringStream&&) = delete;
  /*!
   * \brief Default move assignment deleted.
   */
  StaticStringStream operator=(StaticStringStream&&) = delete;

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Replaces the contents of the underlying string.
   * \param  input Pointer to the character string which shall be set.
   * \throws std::length_error When input length exceeds remaining capacity.
   */
  void str(const char* input) {
    reset();
    append(input);
  }

  /*!
   * \brief  Returns a StringView to the underlying data.
   * \return A string view to the underlying data.
   */
  StringView str() const noexcept { return StringView(data_); }

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Returns a pointer to the underlying array serving as character storage.
   * \return A pointer to the underlying data.
   */
  const char* data() const noexcept { return data_.data(); }

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Append data to the StaticStringStream object.
   * \param  input Pointer to the character string to append.
   * \throws std::length_error When input length exceeds remaining capacity.
   * \return A reference to *this.
   */
  StaticStringStream& append(const char* input) {
    StringView sv{input};
    return this->append(sv);
  }

  /*!
   * \brief  Append data to the StaticStringStream object.
   * \param  input String view to the character string to append.
   * \throws std::length_error When input length exceeds remaining capacity.
   * \return A reference to *this.
   */
  StaticStringStream& append(StringView input) {
    const size_t input_size{input.size()};
    if (input_size < capacity_remaining()) {
      // append data
      data_.append(input.data(), input_size);
    } else {
      vac::language::ThrowOrTerminate<std::length_error>("Input length exceeds remaining capacity");
    }
    return *this;
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.2.2: MD_VAC_A13.2.2_bitwiseOperatorShallReturnBasicValue */
  /*!
   * \brief  Append data to the StaticStringStream object.
   * \param  input Data to be appended.
   * \throws std::length_error When input exceeds remaining capacity.
   * \return A reference to *this.
   */
  StaticStringStream& operator<<(StaticStringStream const& input) {
    append(input.str());
    return *this;
  }

  /*!
   * \brief  Returns the number of characters that can be held in currently allocated storage.
   * \return The number of characters that can be stored.
   */
  size_type capacity() const noexcept { return data_.capacity(); }

  /*!
   * \brief  Returns the remaining number of characters that can be held in currently allocated storage without a reset.
   * \return The remaining number of characters that can be stored.
   */
  size_type capacity_remaining() const noexcept { return data_.capacity() - data_.size(); }

  /*!
   * \brief  Returns the number of characters currently stored.
   * \return The number of stored characters.
   */
  size_type size() const noexcept { return data_.size(); }

  /*!
   * \brief Resets the data position to the beginning, the remaining capacity is then at maximum.
   */
  void reset() noexcept { data_.clear(); }

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Accessor operator.
   * \param  i The index to be accessed.
   * \return The character which is placed at the given index.
   */
  char& operator[](size_type i) { return data_[i]; }

  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \brief  Accessor operator for const.
   * \param  i The index to be accessed.
   * \return The character which is placed at the given index.
   */
  const char& operator[](size_type i) const { return data_[i]; }

  /*!
   * \brief Erases the content of an StaticStringStream positioned at first argument.
   * \param pos Iterator pointing to the cell which should be erased.
   */
  void erase(base_type::iterator pos) { static_cast<void>(data_.erase(pos)); }

  /*!
   * \brief Erases the content of an StaticStringStream from first argument to second argument.
   * \param from The index to start from.
   * \param to The index to end.
   */
  void erase(size_type from, size_type to) { data_.erase(from, to); }

  /*!
   * \brief Erases the content of an StaticStringStream from first argument to second argument.
   * \param first Iterator pointing to the start pos.
   * \param last Iterator pointing to the end pos.
   */
  void erase(base_type::iterator first, base_type::iterator last) { static_cast<void>(data_.erase(first, last)); }

  /*!
   * \brief  Returns an iterator to the first character of the string.
   * \return Iterator to the first character of the string.
   */
  base_type::iterator begin() { return data_.begin(); }

  /*!
   * \brief  Returns an iterator to the character following the last character of the string. This character acts as a
   *         placeholder, attempting to access it results in undefined behavior.
   * \return Iterator to the character following the last character of the string.
   */
  base_type::iterator end() { return data_.end(); }

 private:
  /*!
   * \brief Contains data.
   */
  base_type data_;
};

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_STRING_STREAM_H_
