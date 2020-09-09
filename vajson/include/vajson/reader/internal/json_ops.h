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
/**        \file  json_ops.h
 *        \brief  Collection of all operations on a JsonData
 *      \details  Provides operations for stream based input data
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_JSON_OPS_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_JSON_OPS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "vac/iterators/range.h"
#include "vac/language/throw_or_terminate.h"

#include "vajson/reader/internal/config/json_reader_cfg.h"
#include "vajson/reader/json_data.h"
#include "vajson/reader/parser.h"
#include "vajson/util/json_error_domain.h"

namespace vajson {
inline namespace reader {
namespace internal {

/*!
 * \brief Handles the operations of json data
 */
class JsonOps final {
  /*!
   * \brief A buffer for temporary strings
   */
  using StringBuffer = typename JsonData::Buffer;

 public:
  /*!
   * \brief Json operations constructor
   * \param json_data the JSON data
   */
  explicit JsonOps(JsonData& json_data) noexcept : buffer_(json_data.GetStreamBuffer()) {}

  /*!
   * \brief Deleted copy constructor
   */
  JsonOps(JsonOps const&) = delete;

  /*!
   * \brief Default move constructor
   *
   */
  JsonOps(JsonOps&&) noexcept = default;

  /*!
   * \brief Deleted copy assignment
   */
  JsonOps& operator=(JsonOps const&) & = delete;

  /*!
   * \brief Default move assignment
   */
  JsonOps& operator=(JsonOps&&) & noexcept = default;

  /*!
   * \brief Default Destructor
   */
  ~JsonOps() noexcept = default;

  /*!
   * \brief check if the stream ended
   * \param step the distance from the current position
   * \return true if the current position is out of stream bounds
   */
  auto IsEndOfStream(std::streamsize step = 0) noexcept -> bool { return this->buffer_.get().IsEndOfStream(step); }

  /*!
   * \brief check that the stream has not ended
   * \param step the distance from the current position
   * \return a positive result if the current position is outside stream bounds
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedEOF, if the stream has ended}
   */
  auto CheckNoEndOfStream(std::streamsize step = 0) noexcept -> Result<void> {
    return MakeResult(not this->IsEndOfStream(step), JsonErrc::kUnexpectedEOF, "JsonOps::CheckNoEndOfStream");
  }

  /*!
   * \brief return char at the current position without changing the current position
   * \return the current position
   */
  auto Peek() noexcept -> char8_t {
    if (this->buffer_.get().IsEndOfStream()) {
      vac::language::ThrowOrTerminate<std::invalid_argument>("JsonOps::Peek: File out of bounds");
    } else {
      return this->buffer_.get().Peek();
    }
  }

  /*!
   * \brief move the cursor from the current position to the next position and return current character
   * \return the current character
   */
  auto Take() -> char8_t {
    char8_t c{this->buffer_.get().Peek()};

    static_cast<void>(this->Move());

    return c;
  }

  /*!
   * \brief Tries to take a character
   * \return the character unless the stream has ended
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedEOF, if the stream has ended}
   */
  auto TryTake() -> Result<char8_t> {
    return this->CheckNoEndOfStream().Map([this]() { return this->Take(); });
  };

  /*!
   * \brief move the cursor from the current position to the next position
   */
  auto Move() noexcept -> bool { return this->buffer_.get().Increment(); }

  /*!
   * \brief Get the current position.
   * \return Number of code points read from start.
   */
  auto Tell() noexcept -> std::streamsize { return this->buffer_.get().Tell(); }

  /*!
   * \brief Creates a test function that tests if the passed characters are equal
   * \param ch The character to test
   * \return a function that tests if a character is equal to ch
   */
  static auto IsCharacter(char8_t ch) noexcept -> decltype(auto) {
    return [ch](char8_t c) { return ch == c; };
  }

  /*!
   * \brief Check if the current character fulfills the predicate and execute an action for this character
   * \tparam Predicate The type of the predicate function
   * \tparam Action The type of the action function
   * \param predicate The predicate which decides if the char is to be skipped
   * \param action The action to execute if the character is skipped
   * \returns true if the predicate holds
   * \throw std::invalid_argument If the read is out of bounds.
   */
  template <typename Predicate, typename Action>
  auto DoIf(Predicate&& predicate, Action&& action) noexcept(false) -> bool {
    bool result{false};
    if (this->buffer_.get().IsEndOfStream()) {
      vac::language::ThrowOrTerminate<std::invalid_argument>("JsonOps::PushIf: Access stream out of bounds");
    }
    char8_t cur{this->buffer_.get().Peek()};
    if (predicate(cur)) {
      action(cur);
      result = this->buffer_.get().Increment();
    }
    return result;
  }

  /*!
   * \brief Checks whether the next characters are equal to the passed chars and push them onto the buffer
   * \param buffer The buffer to push onto
   * \param view is the sequence of characters to skip
   * \return true if the compared character sequences are equal
   */
  auto PushIfAny(StringBuffer& buffer, vac::container::CStringView view) -> bool {
    auto predicate = [&view](char8_t ch) { return std::any_of(view.cbegin(), view.cend(), IsCharacter(ch)); };
    auto action = [&buffer](char8_t ch) { buffer.push_back(ch); };
    return this->DoIf(predicate, action);
  }

  /*!
   * \brief check if the current character is equal to c and skip it
   * \param c is the compared character
   * \return true if the compared characters are equal
   */
  auto Skip(char8_t c) -> bool {
    auto do_nothing = [](char8_t) noexcept {};
    return this->DoIf(IsCharacter(c), do_nothing);
  }

  /*!
   * \brief Checks if the next characters are equal to the passed string
   * \param view The string that is expected
   * \param error_msg The error message that is printed in case the check fails
   * \returns nothing
   * \error{JsonErrorDomain, JsonErrc::kInvalidString, if an invalid string was encountered}
   */
  auto SkipString(vac::container::BasicCStringView<char8_t> view, vac::container::CStringView error_msg)
      -> Result<void> {
    Result<void> result{
        MakeResult(not view.empty(), JsonErrc::kInvalidString, "JsonOps::SkipString: Cannot skip empty string")};
    for (char8_t c : view) {
      if (not result) {
        break;
      }
      result = MakeResult(this->Take() == c, JsonErrc::kInvalidString, error_msg.c_str());
    }
    return result;
  }

  /*!
   * \brief Skip characters as long as the predicate holds and executes the action for each character
   * \tparam Predicate The type of the predicate function
   * \tparam Action The type of the action function
   * \param predicate the predicate which decides if the chars are to be skipped
   * \param action The action to perform per character
   * \returns nothing
   */
  template <typename Predicate, typename Action>
  auto DoWhile(Predicate&& predicate,
               Action&& action) noexcept(noexcept(std::declval<Predicate>()(std::declval<char8_t>())) &&
                                         noexcept(std::declval<Action>()(std::declval<char8_t>()))) -> void {
    while (!this->buffer_.get().IsEndOfStream()) {
      char8_t ch{this->buffer_.get().Peek()};
      if (predicate(ch)) {
        action(ch);
      } else {
        break;
      }

      static_cast<void>(this->buffer_.get().Increment());
    }
  }

  /*!
   * \brief skip all allowed kinds of white spaces
   * \returns nothing
   */
  auto SkipWhitespace() noexcept -> void {
    auto predicate = [](char8_t ch) noexcept { return (ch == ' ') || (ch == '\r') || (ch == '\n') || (ch == '\t'); };
    auto do_nothing = [](char8_t) noexcept {};
    this->DoWhile(predicate, do_nothing);
  }

 private:
  /*!
   * \brief StreamBuffer
   */
  std::reference_wrapper<StreamBuffer> buffer_;
};
}  // namespace internal
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_JSON_OPS_H_
