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
/**        \file  stream_buffer.h
 *        \brief  Handles the input stream
 *      \details  e.g. a file or a string stream
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_STREAM_BUFFER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_STREAM_BUFFER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <array>
#include <functional>
#include <istream>
#include <limits>

#include "vajson/reader/internal/config/json_reader_cfg.h"

namespace vajson {
inline namespace reader {
namespace internal {

/*!
 * \brief Handles the input stream
 */
class StreamBuffer final {
 public:
  /*!
   * \brief stream size type
   */
  using size_type = std::streamsize;

  /*!
   * \brief Stream wrapper constructor
   * \param input_stream data stream
   */
  explicit StreamBuffer(std::basic_istream<char8_t>& input_stream) noexcept
      : stream_{input_stream}, buffer_{}, current_buffer_position_{0}, valid_buffer_length_{0}, buffer_counter_{0} {
    static_cast<void>(this->FillBuffer());
  }

  /*!
   * \brief Deleted copy constructor
   */
  StreamBuffer(StreamBuffer const&) = delete;

  /*!
   * \brief Default move constructor
   */
  StreamBuffer(StreamBuffer&&) noexcept = default;

  /*!
   * \brief Deleted copy assignment
   */
  auto operator=(StreamBuffer const&) & -> StreamBuffer& = delete;

  /*!
   * \brief Default move assignment
   */
  auto operator=(StreamBuffer&&) & noexcept -> StreamBuffer& = default;

  /*!
   * \brief Move the stream from the current position to the next position
   * \return true if the stream can be moved, false if not
   */
  auto Increment() noexcept -> bool {
    bool ret_value{false};
    this->current_buffer_position_ += 1;
    if (this->IsValidPosition()) {
      ret_value = true;
    } else {
      this->buffer_counter_ += 1;
      ret_value = this->FillBuffer();
    }

    return ret_value;
  }

  /*!
   * \brief return element at the current position without changing the current position
   * \return value of the current position
   */
  auto Peek() noexcept -> char8_t {
    return this->buffer_[static_cast<std::size_t>(this->current_buffer_position_)]; /* Position is never negative */
  }

  /*!
   * \brief check if the stream ended
   * \param step the distance from the current position
   * \return true if the current position out of stream bounds
   */
  auto IsEndOfStream(size_type step = 0) const noexcept -> bool {
    return (((this->current_buffer_position_ + step) >= this->valid_buffer_length_) && this->stream_.get().eof());
  }

  /*!
   * \brief Get the current position.
   * \return Number of code points read from start.
   */
  auto Tell() noexcept -> size_type {
    return (this->buffer_counter_ * (internal::config::kBufferSize - 1)) + this->current_buffer_position_;
  }

 private:
  /*!
   * \brief Refills the current buffer
   * \returns true if filling was successful
   */
  auto FillBuffer() noexcept -> bool {
    // Check if this is only valid for ASCII
    constexpr static char8_t kEof{std::is_unsigned<char8_t>::value ? std::numeric_limits<char8_t>::max()
                                                                   : std::numeric_limits<char8_t>::min()};

    this->current_buffer_position_ = 0;
    this->stream_.get().read(this->buffer_.data(), internal::config::kBufferSize - 1);
    bool is_good{this->stream_.get().good() || this->stream_.get().eof()};

    this->valid_buffer_length_ = this->stream_.get().gcount();
    this->buffer_[static_cast<std::size_t>(this->valid_buffer_length_)] = kEof;
    return is_good;
  }

  /*!
   * \brief Checks if the current position in the buffer is valid
   * \param offset The offset from the current position
   * \returns true if the current position is valid
   */
  auto IsValidPosition(std::size_t offset = 0) const noexcept -> bool {
    return (this->current_buffer_position_ + static_cast<size_type>(offset)) < this->valid_buffer_length_;
  }

  std::reference_wrapper<std::basic_istream<char8_t>> stream_;  //!< \brief Data stream
  std::array<char8_t, internal::config::kBufferSize> buffer_;   //!< \brief Local data buffer
  size_type current_buffer_position_;                           //!< \brief Current position in the local buffer
  size_type valid_buffer_length_;                               //!< \brief Length of the data stream
  size_type buffer_counter_;                                    //!< \brief Tracks how many buffers have been filled
};
}  // namespace internal
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_STREAM_BUFFER_H_
