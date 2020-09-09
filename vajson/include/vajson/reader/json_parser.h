/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2020 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/**     \file       json_parser.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_JSON_PARSER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_JSON_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "ara/core/string.h"

#include "vajson/reader/strict_parser.h"

namespace vajson {
inline namespace reader {
namespace internal {
/*!
 * \brief Parser that only parses a '{'
 */
class StartObjectParser : public StrictParser<StartObjectParser> {
 public:
  /*!
   * \brief Construct the Parser
   * \param data The JSON data
   */
  explicit StartObjectParser(JsonData& data) : StrictParser{data} {}

  /*!
   * \brief Implements the StartObject callback
   * \return Finished
   */
  static auto OnStartObject() -> ParserState { return ParserState::kFinished; }
};

/*!
 * \brief Parser that only parses a '}'
 */
class EndObjectParser : public StrictParser<EndObjectParser> {
 public:
  /*!
   * \brief Construct the Parser
   * \param data The JSON data
   */
  explicit EndObjectParser(JsonData& data) : StrictParser{data} {}

  /*!
   * \brief Implements the EndObject callback
   * \return Finished
   */
  static auto OnEndObject(std::size_t) -> ParserState { return ParserState::kFinished; }
};

/*!
 * \brief Parser that only parses a '['
 */
class StartArrayParser : public StrictParser<StartArrayParser> {
 public:
  /*!
   * \brief Construct the Parser
   * \param data The JSON data
   */
  explicit StartArrayParser(JsonData& data) : StrictParser{data} {}

  /*!
   * \brief Implements the StartArray callback
   * \return Finished
   */
  static auto OnStartArray() -> ParserState { return ParserState::kFinished; }
};

/*!
 * \brief Parser that only parses a ']'
 */
class EndArrayParser : public StrictParser<EndArrayParser> {
 public:
  /*!
   * \brief Construct the Parser
   * \param data The JSON data
   */
  explicit EndArrayParser(JsonData& data) : StrictParser{data} {}

  /*!
   * \brief Implements the EndArray callback
   * \return Finished
   */
  static auto OnEndArray(std::size_t) -> ParserState { return ParserState::kFinished; }
};

/*!
 * \brief Just wraps a non-modified JsonParser so that it can be used via composition instead of inheritance
 */
class CompositionParser final : public StrictParser<CompositionParser> {
 public:
  /*!
   * \brief Construct the Parser
   * \param data The JSON data
   */
  explicit CompositionParser(JsonData& data) : StrictParser{data} {}
};
}  // namespace internal

/*!
 * \brief Implement a parser that can be used to implement stateful JSON files
 *
 * \vpublic
 */
class JsonParser final {
  /*!
   * \brief Unqualified access to the parser state
   */
  using ParserState = vajson::reader::ParserState;

 public:
  /*!
   * \brief Construct the Parser
   * \param data The JSON data
   */
  explicit JsonParser(JsonData& data) : parser_{data}, data_{data}, result_{}, customized_{false} {}

  /*!
   * \brief Gets the parser result after all parsing is done
   * \param state The state to return after a successful parse
   * \return Either state or the error
   */
  auto Finish(ParserState state = ParserState::kFinished) -> Result<ParserState> {
    return this->result_.Map([&state]() { return state; });
  }

  /*!
   * \brief Checks if the next token is a key
   * \tparam Fn The function type
   * \param fn The function to execute on the key value
   * \return a reference to itself
   */
  template <typename Fn>
  auto Key(Fn&& fn) -> JsonParser& {
    return this->IfValid([this, &fn]() { return this->parser_.ParseKey(std::forward<Fn>(fn)).Drop(); });
  }

  /*!
   * \brief Checks if the next token is the expected key
   * \param key The key to compare.
   * \return a reference to itself
   */
  auto Key(vac::container::CStringView key) -> JsonParser& {
    return this->IfValid([this, &key]() { return this->parser_.CheckKey(key).Drop(); });
  }

  /*!
   * \brief Checks if the next token is the start of an object
   * \return a reference to itself
   */
  auto StartObject() -> JsonParser& {
    return this->IfValid([this]() { return internal::StartObjectParser(this->data_).Parse().Drop(); });
  }

  /*!
   * \brief Checks if the next token is the end of an object
   * \return a reference to itself
   */
  auto EndObject() -> JsonParser& {
    return this->IfValid([this]() { return internal::EndObjectParser(this->data_).Parse().Drop(); });
  }

  /*!
   * \brief Checks if the next token is the start of an array
   * \return a reference to itself
   */
  auto StartArray() -> JsonParser& {
    return this->IfValid([this]() { return internal::StartArrayParser(this->data_).Parse().Drop(); });
  }

  /*!
   * \brief Checks if the next token is the end of an array
   * \return a reference to itself
   */
  auto EndArray() -> JsonParser& {
    return this->IfValid([this]() { return internal::EndArrayParser(this->data_).Parse().Drop(); });
  }

  /*!
   * \brief Checks if the next token is a bool
   * \tparam Fn The function type
   * \param fn The function to execute on the bool value
   * \return a reference to itself
   */
  template <typename Fn>
  auto Bool(Fn&& fn) -> JsonParser& {
    return this->IfValid([this, &fn]() { return this->parser_.ParseBool(std::forward<Fn>(fn)).Drop(); });
  }

  /*!
   * \brief Checks if the next token is a string
   * \tparam Fn The function type
   * \param fn The function to execute on the string value
   * \return a reference to itself
   */
  template <typename Fn>
  auto String(Fn&& fn) -> JsonParser& {
    return this->IfValid([this, &fn]() { return this->parser_.ParseString(std::forward<Fn>(fn)).Drop(); });
  }

  /*!
   * \brief Checks if the next token is a number
   * \tparam Num The number type
   * \tparam Fn The function type
   * \param fn The function to execute on the number value
   * \return a reference to itself
   */
  template <typename Num, typename Fn>
  auto Number(Fn&& fn) -> JsonParser& {
    return this->IfValid([this, &fn]() { return this->parser_.ParseNumber<Num>(std::forward<Fn>(fn)).Drop(); });
  }

  /*!
   * \brief Checks if the next token is an array
   * \tparam Fn The function type
   * \param fn The function to execute on the array values
   * \return a reference to itself
   */
  template <typename Fn>
  auto Array(Fn&& fn) -> JsonParser& {
    return this->IfValid([this, &fn]() { return this->parser_.ParseArray(std::forward<Fn>(fn)).Drop(); });
  }

  /*!
   * \brief Checks if the next token is an array of strings
   * \tparam Fn The function type
   * \param fn The function to execute on the string values
   * \return a reference to itself
   */
  template <typename Fn>
  auto StringArray(Fn&& fn) -> JsonParser& {
    return this->IfValid([this, &fn]() { return this->parser_.ParseStringArray(std::forward<Fn>(fn)).Drop(); });
  }

  /*!
   * \brief Checks if the next token is an array of numbers
   * \tparam Fn The function type
   * \param fn The function to execute on the number values
   * \return a reference to itself
   */
  template <typename Num, typename Fn>
  auto NumberArray(Fn&& fn) -> JsonParser& {
    return this->IfValid([this, &fn]() { return this->parser_.ParseNumberArray<Num>(std::forward<Fn>(fn)).Drop(); });
  }

  /*!
   * \brief Will enhance the error that is encountered by a more detailed message
   * \param msg The provided message that at least live until the ParserResult is evaluated!
   * \return a reference to itself
   */
  auto AddErrorInfo(CStr msg) -> JsonParser& {
    if (not this->customized_ && not this->result_) {
      ErrorCode ec{this->result_.Error().WithUserMessage(msg)};
      this->result_.EmplaceError(ec);
      this->customized_ = true;
    }
    return *this;
  }

  /*!
   * \brief Will enhance the error that is encountered by a more detailed message
   * \param msg The provided message that at least live until the ParserResult is evaluated!
   * \return a reference to itself
   */
  auto AddErrorInfo(vac::container::CStringView msg) -> JsonParser& { return AddErrorInfo(msg.c_str()); }

  /*!
   * \brief Will replace the error that is encountered by a custom error
   * \tparam The constructor arguments for the error code
   * \param args The arguments to construct an error code with
   * \return a reference to itself
   */
  template <typename... Args>
  auto AddErrorInfo(Args... args) -> JsonParser& {
    if (not this->customized_ && not this->result_) {
      this->result_.EmplaceError(ErrorCode{std::forward<Args>(args)...});
      this->customized_ = true;
    }
    return *this;
  }

 private:
  /*!
   * \brief Execute and save result if the previous actions were successful
   * \tparam Fn The function type
   * \param fn The function to execute
   * \return a reference to itself
   */
  template <typename Fn>
  auto IfValid(Fn&& fn) -> JsonParser& {
    if (this->result_) {
      this->result_ = std::forward<Fn>(fn)();
    }
    return *this;
  }

  /*!
   * \brief The internal parser
   */
  internal::CompositionParser parser_;

  /*!
   * \brief The JSON data
   */
  vajson::reader::JsonData& data_;

  /*!
   * \brief The state of the parse containing the first occurred error
   */
  Result<void> result_;

  /*!
   * \brief Flag if the error message already has been customized
   */
  bool customized_;
};
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_JSON_PARSER_H_
