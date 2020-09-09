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
/*!       \file     parser.h
 *        \brief    TODO
 *
 *        \details  TODO
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_PARSER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <string>
#include <utility>

#include "ara/core/string_view.h"
#include "vac/container/string_literals.h"
#include "vac/iterators/range.h"

#include "vajson/reader/internal/json_ops.h"
#include "vajson/reader/json_document.h"
#include "vajson/util/json_error_domain.h"
#include "vajson/util/types.h"

namespace vajson {
inline namespace reader {
/*!
 * SAX-style JSON parser. Use \ref Parser for UTF8 encoding and default allocator.
 * GenericReader parses JSON text from a stream, and send events synchronously to an
 *  object implementing Handler concept.
 *  It needs to allocate a stack for storing a single decoded string during
 *  non-destructive parsing.
 *  For in-situ parsing, the decoded string is directly written to the source
 *  text string, no temporary buffer is required.
 *  A GenericReader object can be reused for parsing multiple JSON text.
 *
 *  \tparam Child The parser type which is called for every event that it implements
 *  \tparam Char The character type according to the encoding style
 *
 *  \vprivate Vector component internal
 */
template <typename Child>
class Parser {
  /*!
   * \brief A buffer for temporary strings
   */
  using StringBuffer = typename JsonData::Buffer;

 public:
  /*!
   * \brief A String view for the current encoding with a terminating null
   */
  using CStringView = vac::container::BasicCStringView<char8_t>;

  /*!
   *  \brief Number type
   */
  using Num = util::JsonNumber;

  /*!
   * \brief Document Type
   */
  using Document = JsonData;

  /*!
   * \brief Provides unqualified access to the ParserState
   */
  using ParserState = ::vajson::reader::ParserState;

  /*!
   * \brief Represents the current state of the parser
   */
  using ParserResult = Result<ParserState>;

  /*!
   * \brief constructor
   * \param doc The JSON document to parse
   */
  explicit Parser(Document& doc) noexcept : document_{doc}, json_ops_(doc) {}

  /*!
   * \brief Deleted copy constructor
   */
  Parser(Parser const&) = delete;

  /*!
   * \brief Deleted copy assignment
   */
  auto operator=(Parser const&) & -> Parser& = delete;

  /*!
   * \brief Default destructor
   */
  virtual ~Parser() noexcept = default;

  /*!
   * \brief Parse file until the current parser is finished
   * \details ParserState will be finished and should be used on the top level only.
   * \return the ParserState of Finished
   *
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if an unknown value has been encountered}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedEOF, if the stream has ended}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBrackets, if no '[' was expected}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBraces, if no '{' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedClosingBrackets, if ']' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedClosingBraces, if '}' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedValue, if a value was expected}
   * \error{JsonErrorDomain, JsonErrc::kInvalidString, if an invalid string was encountered}
   * \error{JsonErrorDomain, JsonErrc::kInvalidType, if an invalid/unknown type was encountered}
   * \error{JsonErrorDomain, JsonErrc::kNotInObject, if in an array or on toplevel}
   * \error{JsonErrorDomain, JsonErrc::kNotInArray, if in an object or on toplevel}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOnTopLevel, if not in an object or array}
   * \error{JsonErrorDomain, JsonErrc::kUnicodeEscape, if a unicode escape was encountered}
   */
  auto Parse() -> ParserResult {
    // Detect if the passed document is empty
    this->GetJsonOps().SkipWhitespace();

    // Run parser
    ParserResult result{this->GetJsonOps().CheckNoEndOfStream().Map([]() { return ParserState::kRunning; })};
    while (result == ParserState::kRunning) {
      result = this->ParseValue();
    }

    // In case of an error, add the current location to the support data
    return result.MapError([this](ErrorCode ec) {
      ec.SetSupportData(static_cast<vajson::ErrorDomain::SupportDataType>(this->json_ops_.Tell()));
      return ec;
    });
  }

  /*!
   * \brief Parse file until the current parse is finished
   * \details Parserstate will be ready for further parsing.
   * \return the ParserState of Running
   *
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if an unknown value has been encountered}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedEOF, if the stream has ended}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBrackets, if no '[' was expected}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBraces, if no '{' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedClosingBrackets, if ']' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedClosingBraces, if '}' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedValue, if a value was expected}
   * \error{JsonErrorDomain, JsonErrc::kInvalidString, if an invalid string was encountered}
   * \error{JsonErrorDomain, JsonErrc::kInvalidType, if an invalid/unknown type was encountered}
   * \error{JsonErrorDomain, JsonErrc::kNotInObject, if in an array or on toplevel}
   * \error{JsonErrorDomain, JsonErrc::kNotInArray, if in an object or on toplevel}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOnTopLevel, if not in an object or array}
   * \error{JsonErrorDomain, JsonErrc::kUnicodeEscape, if a unicode escape was encountered}
   */
  auto SubParse() -> ParserResult {
    return this->Parse().Map([](ParserState) { return ParserState::kRunning; });
  }

 protected:
  /*!
   * \brief Default move constructor
   */
  Parser(Parser&&) noexcept = default;

  /*!
   * \brief Default move assignment
   */
  auto operator=(Parser&&) & noexcept -> Parser& = default;

  /*!
   *  \brief Returns the current key
   */
  auto GetCurrentKey() const -> CStringView { return this->GetJsonDocument().GetCurrentKey(); }

  /*!
   *  \brief Returns a reference to the Json File
   */
  auto GetJsonDocument() noexcept -> Document& { return this->document_.get(); }

  /*!
   *  \brief Returns a reference to the Json File
   */
  auto GetJsonDocument() const noexcept -> Document const& { return this->document_.get(); }

  // VECTOR NC AutosarC++17_10-A5.2.3: MD_JSON_AutosarC++17_10-A5.2.3_f_bounded_polymorphism
  /*!
   * \brief Access the CRTP child
   * \return The CRTP child for this parser
   * \vprivate
   */
  auto GetChild() noexcept -> Child& { return *static_cast<Child*>(this); }

 private:
  // Default implementations to fall back to!
  /*!
   * \brief Default event for Null Event
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto Null() -> ParserResult { return this->GetChild().OnUnexpectedEvent(); }
  /*!
   * \brief Default event for Bools
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto Bool(bool) -> ParserResult { return this->GetChild().OnUnexpectedEvent(); }
  /*!
   * \brief Default event for Numbers
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto Number(util::JsonNumber) -> ParserResult { return this->GetChild().OnUnexpectedEvent(); }
  /*!
   * \brief Default event for Strings
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto String(CStringView) -> ParserResult { return this->GetChild().OnUnexpectedEvent(); }
  /*!
   * \brief Default event for Keys
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto Key(CStringView) -> ParserResult { return this->GetChild().OnUnexpectedEvent(); }
  /*!
   * \brief Default event for the start of objects
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto StartObject() -> ParserResult { return this->GetChild().OnUnexpectedEvent(); }
  /*!
   * \brief Default event for the end of objects
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto EndObject(std::size_t) -> ParserResult { return this->GetChild().OnUnexpectedEvent(); }
  /*!
   * \brief Default event for the start of arrays
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto StartArray() -> ParserResult { return this->GetChild().OnUnexpectedEvent(); }
  /*!
   * \brief Default event for the end of arrays
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto EndArray(std::size_t) -> ParserResult { return this->GetChild().OnUnexpectedEvent(); }

  /*!
   * \brief Default event for Null Event
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto OnNull() -> ParserResult { return ParserResult{this->GetChild().Null()}; }
  /*!
   * \brief Default event for Bools
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto OnBool(bool b) -> ParserResult { return ParserResult{this->GetChild().Bool(b)}; }
  /*!
   * \brief Default event for Numbers
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto OnNumber(util::JsonNumber n) -> ParserResult { return ParserResult{this->GetChild().Number(n)}; }
  /*!
   * \brief Default event for Strings
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto OnString(CStringView s) -> ParserResult { return ParserResult{this->GetChild().String(s)}; }
  /*!
   * \brief Default event for Keys
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto OnKey(CStringView k) -> ParserResult { return ParserResult{this->GetChild().Key(k)}; }
  /*!
   * \brief Default event for the start of objects
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto OnStartObject() -> ParserResult { return ParserResult{this->GetChild().StartObject()}; }
  /*!
   * \brief Default event for the end of objects
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto OnEndObject(std::size_t n) -> ParserResult { return ParserResult{this->GetChild().EndObject(n)}; }
  /*!
   * \brief Default event for the start of arrays
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto OnStartArray() -> ParserResult { return ParserResult{this->GetChild().StartArray()}; }
  /*!
   * \brief Default event for the end of arrays
   * \param n The number of arrays
   * \returns running if use of default methods is allowed
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if using default methods is not allowed}
   */
  auto OnEndArray(std::size_t n) -> ParserResult { return ParserResult{this->GetChild().EndArray(n)}; }
  /*!
   * \brief Default event for commas in arrays or objects
   * \return running
   */
  auto OnComma() -> ParserResult { return ParserResult{ParserState::kRunning}; }

  /*!
   * \brief Evaluate action when default method is called
   * \returns running
   */
  auto OnUnexpectedEvent() -> ParserResult { return ParserResult{ParserState::kRunning}; }

  /*!
   * \brief parse "null" value
   * \returns the state that is returned by the user.
   * \error{JsonErrorDomain, JsonErrc::kInvalidString, if an invalid string was encountered}
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   */
  auto ParseNull() -> ParserResult {
    using namespace vac::container::literals;  // NOLINT(build/namespaces)

    return (this->GetJsonOps().SkipString("null"_sv, "Parser::ParseNull: Expected 'null'"_sv))
        .AndThen([this]() { return this->GetState().AddValue(); })
        .AndThen([this]() { return ParserResult{this->GetChild().OnNull()}; });
  }

  /*!
   * \brief parse "true" value
   * \returns the state that is returned by the user.
   * \error{JsonErrorDomain, JsonErrc::kInvalidString, if an invalid string was encountered}
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   */
  auto ParseTrue() -> ParserResult {
    using namespace vac::container::literals;  // NOLINT(build/namespaces)

    return (this->GetJsonOps().SkipString("true"_sv, "Parser::ParseTrue: Expected 'true'"_sv))
        .AndThen([this]() { return this->GetState().AddValue(); })
        .AndThen([this]() { return ParserResult{this->GetChild().OnBool(true)}; });
  }

  /*!
   * \brief parse "false" value
   * \returns the state that is returned by the user.
   * \error{JsonErrorDomain, JsonErrc::kInvalidString, if an invalid string was encountered}
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   */
  auto ParseFalse() -> ParserResult {
    using namespace vac::container::literals;  // NOLINT(build/namespaces)

    return (this->GetJsonOps().SkipString("false"_sv, "Parser::ParseFalse: Expected 'false'"_sv))
        .AndThen([this]() { return this->GetState().AddValue(); })
        .AndThen([this]() { return ParserResult{this->GetChild().OnBool(false)}; });
  }

  /*!
   * \brief parse numbers by using standard functions
   * \returns the state that is returned by the user.
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   */
  auto ParseNumber(util::NumberBase base) -> ParserResult {
    return (this->GetState().AddValue())
        .Map([this, &base]() { return this->GetNumber(base); })
        .AndThen([this, base](CStringView sv) { return ParserResult{this->GetChild().OnNumber(Num(sv, base))}; });
  }

  /*!
   * \brief Parse string and generate String event.
   * \returns the state that is returned by the user.
   * \error{JsonErrorDomain, JsonErrc::kInvalidString, if an invalid string was encountered}
   * \error{JsonErrorDomain, JsonErrc::kUnicodeEscape, if a unicode escape was encountered}
   * \error{JsonErrorDomain, JsonErrc::kExpectedValue, if a value was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   */
  auto ParseString() -> ParserResult {
    ParserResult result{ParserState::kRunning};

    Result<CStringView> string_result{GetUnescapedString()};
    if (string_result) {
      CStringView string{*string_result};
      this->GetJsonOps().SkipWhitespace();
      if (this->GetJsonOps().Skip(':')) {
        result = this->GetState().AddKey().AndThen([this, &string]() {
          this->GetJsonDocument().StoreCurrentKey(string);
          return ParserResult{this->GetChild().OnKey(string)};
        });
      } else {
        result = this->GetState().AddValue().AndThen(
            [this, &string]() { return ParserResult{this->GetChild().OnString(string)}; });
      }
    } else {
      result.EmplaceError(string_result.Error());
    }

    return result;
  }

  /*!
   * \brief Reads in a JSON string and removes JSON escapes
   * \return A string view into the parsed string
   * \error{JsonErrorDomain, JsonErrc::kInvalidString, if an invalid string was encountered}
   * \error{JsonErrorDomain, JsonErrc::kUnicodeEscape, if a unicode escape was encountered}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedEOF, if the stream has ended}
   */
  auto GetUnescapedString() -> Result<CStringView> {
    static_cast<void>(this->GetJsonOps().Move());  // Take quote
    StringBuffer& buffer{this->GetJsonDocument().GetClearedStringBuffer()};

    Result<char8_t> result{'\0'};
    while (result && (result = this->GetJsonOps().TryTake())) {
      char8_t cur{*result};
      if (cur == '"') {
        break;
      } else if (cur == '\\') {
        /* Given an escape character we determine the kind of escape */
        cur = this->GetJsonOps().Take();
        switch (cur) {
          case 'b':
            buffer.push_back('\b');
            break;
          case 'f':
            buffer.push_back('\f');
            break;
          case 'n':
            buffer.push_back('\n');
            break;
          case 'r':
            buffer.push_back('\r');
            break;
          case 't':
            buffer.push_back('\t');
            break;
          case 'u':
            result.EmplaceError(JsonErrc::kUnicodeEscape, "\\u notation is not supported!");
            break;
          default:
            buffer.push_back(cur);
            break;
        }
      } else {
        buffer.push_back(cur);
      }
    }

    return result.Filter([](char8_t c) { return c == '"'; }, JsonErrc::kInvalidString).Map([this](char8_t) {
      return this->GetJsonDocument().GetCurrentString();
    });
  }

  /*!
   * \brief Parse the Start of an Object
   * \returns the state that is returned by the user.
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBraces, if no '{' was expected}
   */
  auto ParseStartObject() -> ParserResult {
    static_cast<void>(this->GetJsonOps().Move());

    return this->GetState().AddValue().AndThen([this]() { return this->GetState().AddObject(); }).AndThen([this]() {
      return ParserResult{this->GetChild().OnStartObject()};
    });
  }

  /*!
   * \brief Parse the End of an Object
   * \returns the state that is returned by the user.
   * \error{JsonErrorDomain, JsonErrc::kNotInObject, if in an array or on toplevel}
   * \error{JsonErrorDomain, JsonErrc::kExpectedValue, If a value is expected}
   */
  auto ParseEndObject() -> ParserResult {
    static_cast<void>(this->GetJsonOps().Move());

    return (this->GetState().PopObject()).AndThen([this](std::size_t count) {
      return ParserResult{this->GetChild().OnEndObject(count)};
    });
  }

  /*!
   * \brief Parse the Start of an Object
   * \returns the state that is returned by the user.
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBrackets, if no '[' was expected}
   */
  auto ParseStartArray() -> ParserResult {
    static_cast<void>(this->GetJsonOps().Move());

    return this->GetState().AddValue().AndThen([this]() { return this->GetState().AddArray(); }).AndThen([this]() {
      return ParserResult{this->GetChild().OnStartArray()};
    });
  }

  /*!
   * \brief Parse the End of an Object
   * \returns the state that is returned by the user.
   * \error{JsonErrorDomain, JsonErrc::kNotInArray, if in an object or on toplevel}
   */
  auto ParseEndArray() -> ParserResult {
    static_cast<void>(this->GetJsonOps().Move());

    return this->GetState().PopArray().AndThen(
        [this](std::size_t count) { return ParserResult{this->GetChild().OnEndArray(count)}; });
  }

  /*!
   * \brief Parse a comma in a list/object
   * \return running if the parser is in an object or array
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOnTopLevel, if not in an object or array}
   */
  auto ParseComma() -> ParserResult {
    return this->GetState().CheckNonEmpty().AndThen([this]() {
      static_cast<void>(this->GetJsonOps().Move());
      return ParserResult{this->GetChild().OnComma()};
    });
  }

  /*!
   * \brief Parse any JSON value
   * \return Either running of finished
   * \error{JsonErrorDomain, JsonErrc::kUserValidationFailed, if an unknown value has been encountered}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedEOF, if the stream has ended}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBrackets, if no '[' was expected}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBraces, if no '{' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedClosingBrackets, if ']' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedClosingBraces, if '}' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedValue, if a value was expected}
   * \error{JsonErrorDomain, JsonErrc::kInvalidString, if an invalid string was encountered}
   * \error{JsonErrorDomain, JsonErrc::kInvalidType, if an invalid/unknown type was encountered}
   * \error{JsonErrorDomain, JsonErrc::kNotInObject, if in an array or on toplevel}
   * \error{JsonErrorDomain, JsonErrc::kNotInArray, if in an object or on toplevel}
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOnTopLevel, if not in an object or array}
   * \error{JsonErrorDomain, JsonErrc::kUnicodeEscape, if a unicode escape was encountered}
   */
  auto ParseValue() -> ParserResult {
    this->GetJsonOps().SkipWhitespace();

    ParserResult result{ParserState::kFinished};
    if (this->GetJsonOps().IsEndOfStream()) {
      result = result.Filter([this](ParserState const&) { return this->GetState().CheckEndOfFile(); });
    } else {
      static_cast<void>(this->GetJsonDocument().GetClearedStringBuffer());
      switch (this->GetJsonOps().Peek()) {
        case 'n':
          result = this->ParseNull();
          break;
        case 't':
          result = this->ParseTrue();
          break;
        case 'f':
          result = this->ParseFalse();
          break;
        case '"':
          result = this->ParseString();
          break;
        case '{':
          result = this->ParseStartObject();
          break;
        case '}':
          result = this->ParseEndObject();
          break;
        case '[':
          result = this->ParseStartArray();
          break;
        case ']':
          result = this->ParseEndArray();
          break;
        case ',':
          result = this->ParseComma();
          break;
        case '-': {
          this->GetJsonDocument().GetStringBuffer().push_back('-');
          static_cast<void>(this->GetJsonOps().Move());
        }
        /* Fallthrough */
        case '0': {
          util::NumberBase base{this->ParseNumberBase(this->GetJsonDocument().GetStringBuffer())};
          result = this->ParseNumber(base);
          break;
        }
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          result = this->ParseNumber(util::NumberBase::kDecimal);
          break;
        default:
          result.EmplaceError(JsonErrc::kInvalidType, "ParseValue: Expected a valid JSON token.");
          break;
      }
    }
    return result;
  }

  /*!
   * \brief Read a (more or less) valid number
   * \returns the parsed Number string
   */
  auto GetNumber(util::NumberBase base) noexcept -> CStringView {
    CStringView result{(base == util::NumberBase::kZeroOnly) ? GetZeroNumber() : GetNonZeroNumber(base)};
    assert(not result.empty());
    return result;
  }

  /*!
   * \brief Read a zero
   * \returns the parsed Number string
   */
  auto GetZeroNumber() noexcept -> CStringView { return this->GetJsonDocument().GetCurrentString(); }

  /*!
   * \brief Read a (more or less) valid non-zero number
   * \returns the parsed Number string
   */
  auto GetNonZeroNumber(util::NumberBase base) -> CStringView {
    using namespace vac::container::literals;  // NOLINT(build/namespaces)

    constexpr static CStringView kAllowedChars{".Ee-+"_sv};
    StringBuffer& buffer{this->GetJsonDocument().GetStringBuffer()};
    auto action = [&buffer](char8_t ch) { buffer.push_back(ch); };

    auto predicate = [this, base](char8_t ch) {
      return util::internal::IsDigit(ch, base) ||
             std::any_of(kAllowedChars.cbegin(), kAllowedChars.cend(), [ch](char8_t c) { return ch == c; });
    };
    this->GetJsonOps().DoWhile(predicate, action);

    // Create a string view from the number-string
    return this->GetJsonDocument().GetCurrentString();
  }

  /*!
   * \brief Returns the base for the number
   */
  auto ParseNumberBase(typename Document::Buffer& buffer) -> util::NumberBase {
    using namespace vac::container::literals;  // NOLINT(build/namespaces)

    util::NumberBase base{util::NumberBase::kDecimal};
    if (this->GetJsonOps().Peek() == '0') {
      static_cast<void>(this->GetJsonOps().Move());

      buffer.push_back('0');

      if (this->GetJsonOps().PushIfAny(buffer, "xX"_sv)) {
        base = util::NumberBase::kHex;
      } else if (this->GetJsonOps().PushIfAny(buffer, ".eE"_sv)) {
        /* Floats can also start with 0, but are always decimal */
        base = util::NumberBase::kDecimal;
      } else if (this->GetJsonOps().PushIfAny(buffer, "1234567"_sv)) {
        base = util::NumberBase::kOctal;
      } else {
        base = util::NumberBase::kZeroOnly;
      }
    }
    return base;
  }

  /*!
   * \brief Returns the Json structure state
   */
  auto GetState() -> internal::DepthCounter& { return this->GetJsonDocument().GetState(); }
  /*!
   * \brief Returns reference to the encoded file
   */
  auto GetJsonOps() noexcept -> internal::JsonOps& { return this->json_ops_; }

  /*!
   * \brief Json document
   */
  std::reference_wrapper<Document> document_;

  /*!
   * \brief Operations on json document
   */
  internal::JsonOps json_ops_;
};
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_PARSER_H_
