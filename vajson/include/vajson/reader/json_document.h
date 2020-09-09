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
/*!       \file     json_document.h
 *        \brief    TODO
 *
 *        \details  TODO
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_JSON_DOCUMENT_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_JSON_DOCUMENT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <array>
#include <istream>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "vac/container/c_string_view.h"

#include "vajson/reader/internal/config/json_reader_cfg.h"
#include "vajson/reader/internal/depth_counter.h"
#include "vajson/reader/internal/stream_buffer.h"
#include "vajson/reader/parser_state.h"
#include "vajson/util/json_error_domain.h"
#include "vajson/util/number.h"
#include "vajson/util/types.h"

namespace vajson {
inline namespace reader {
namespace internal {
/*!
 * \brief Forward declaration for JsonOps
 */
class JsonOps;
}  // namespace internal

/*!
 * \brief Handles the state of the JSON document
 * \details State: Position in file, Nesting of JSON tree
 *
 * \vprivate Vector component internal
 */
template <typename Char = char8_t>
class JsonDocument final {
 public:
  /*!
   * \brief Class JsonOps must have access to the StreamBuffer but no other classes.
   */
  friend class internal::JsonOps;  // VECTOR SL AutosarC++17_10-A11.3.1: MD_JSON_AutosarC++17_10-A11.3.1_json_ops
  /*!
   * \brief The C string view specialized for the encoding
   */
  using CStringView = vac::container::BasicCStringView<char8_t>;

  /*!
   * \brief Abstraction for the buffer used here
   * \todo Replace with array string
   */
  using Buffer = std::basic_string<char8_t>;

  // VECTOR NC AutosarC++17_10-A15.4.5: MD_JSON_AutosarC++17_10-A15.4.5_static_vector
  /*!
   * \brief Instantiate using a constructed reader
   */
  explicit JsonDocument(std::basic_istream<char8_t>& input_stream) noexcept
      : stream_buffer_{input_stream}, depth_counter_{internal::config::kMaxDepth}, current_key_{}, current_buffer_{} {
    this->current_key_.reserve(internal::config::kKeyBufferSize);
    this->current_buffer_.reserve(internal::config::kStringBufferSize);
  }

  /*!
   * \brief Deleted copy constructor
   */
  JsonDocument(JsonDocument const&) = delete;

  // VECTOR NC AutosarC++17_10-A15.5.1: MD_JSON_AutosarC++17_10-A15.5.1_std_no_noexcept_move
  /*!
   * \brief Default move constructor
   */
  JsonDocument(JsonDocument&&) = default;  // NOLINT

  /*!
   * \brief Deleted copy assignment
   */
  auto operator=(JsonDocument const&) & -> JsonDocument& = delete;

  // VECTOR NC AutosarC++17_10-A15.5.1: MD_JSON_AutosarC++17_10-A15.5.1_std_no_noexcept_move
  /*!
   * \brief Default move assignment
   */
  auto operator=(JsonDocument&&) & -> JsonDocument& = default;  // NOLINT

  /*!
   * \brief Default destructor
   */
  ~JsonDocument() noexcept = default;

  /*!
   * \brief Returns the Json structure state
   */
  auto GetState() noexcept -> internal::DepthCounter& { return this->depth_counter_; }

  /*!
   * \brief Refreshes the current key
   * \param key the key to store
   * \returns
   */
  auto StoreCurrentKey(CStringView key) -> void { this->current_key_ = key.ToString(std::allocator<char8_t>()); }

  /*!
   * \brief Returns the current key
   * \returns a string view into the current key
   */
  auto GetCurrentKey() const noexcept -> CStringView { return CStringView::FromString(this->current_key_); }

  /*!
   * \brief Returns the current string
   * \returns a string view into the current string
   */
  auto GetCurrentString() const noexcept -> CStringView { return CStringView::FromString(this->current_buffer_); }

  /*!
   * \brief Empties the string buffer and returns it
   * \returns a buffer for the current string
   */
  auto GetClearedStringBuffer() noexcept -> Buffer& {
    this->current_buffer_.clear();
    return this->current_buffer_;
  }

  /*!
   * \brief Returns the string buffer
   * \returns a buffer for the current string
   */
  auto GetStringBuffer() noexcept -> Buffer& { return this->current_buffer_; }

 private:
  /*!
   * \brief Returns the stream buffer
   */
  auto GetStreamBuffer() noexcept -> internal::StreamBuffer& { return this->stream_buffer_; }

  internal::StreamBuffer stream_buffer_;  //!< \brief Access to the data stream
  internal::DepthCounter depth_counter_;  //!< \brief Json structure state
  Buffer current_key_;                    //!< \brief Contains the current key
  Buffer current_buffer_;                 //!<  \brief Contains the current buffer
};
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_JSON_DOCUMENT_H_
