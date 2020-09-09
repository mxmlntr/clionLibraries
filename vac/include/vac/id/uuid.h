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
/*!     \file     uuid.h
 *      \brief    Simple data structure for UUID representation and comparison.
 *
 *      \details  Parser and data structure for representation of a UUID in 8-4-4-4-12 notation
 *                e.g. 12345678-abcd-9876-fedc-123456789abc
 *                UUIDs are required to match this format and to use lowercase digits.
 *                Note: There are no further requirements for the content / format of the UUID.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_ID_UUID_H_
#define LIB_VAC_INCLUDE_VAC_ID_UUID_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <array>
#include <string>
#include "ara/core/result.h"
#include "ara/core/span.h"
#include "ara/core/string_view.h"
#include "vac/container/basic_string.h"
#include "vac/container/static_string_stream.h"
#include "vac/container/static_vector.h"
#include "vac/testing/test_adapter.h"

namespace vac {
namespace id {

/* VECTOR Next Construct AutosarC++17_10-M3.4.1: MD_VAC_M3.4.1_symbolsCanBeDeclaredLocally */
/*!
 * \brief UUID data structure.
 * \trace CREQ-158646
 */
class UUID final {
  /*!
   * \brief Length of an UUID string in 8-4-4-4-12 representation.
   */
  constexpr static size_t kUuidStringLength{8 + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 12};

  /*!
   * \brief Byte length of an UUID.
   */
  constexpr static uint32_t kUuidByteLength{16};

 public:
  /*!
   * \brief StringView Alias for ara::core::StringView.
   */
  using StringView = ara::core::StringView;
  /*!
   * \brief Typedef for raw data of UUID.
   */
  using UUIDData = ara::core::Span<const uint8_t>;

  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef
  /*!
   * \brief Typedef for string representation of UUID.
   */
  using UUIDString = std::array<char, kUuidStringLength>;

  /*!
   * \brief Generates an UUID based on the input string. The input must match the format
   *        expression [0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}
   *        e.g. 12345678-abcd-9876-fedc-123456789abc.
   * \param input String representation of the UUID.
   * \trace CREQ-158647
   */
  explicit UUID(StringView input);

  /*!
   * \brief  Returns a Result<UUID> based on the input string. The input must match the format
   *         expression [0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}
   *         e.g. 12345678-abcd-9876-fedc-123456789abc.
   *         An invalid string will return PosixErrc::invalid_argument.
   * \param  input String representation of the UUID.
   * \return Result with UUID if valid format, otherwise the error code.
   * \trace  CREQ-158647
   */
  static ara::core::Result<UUID> FromString(StringView input);

  /*!
   * \brief Generates an empty UUID containing zeros.
   */
  UUID() noexcept = default;

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*!
   * \brief Destructor.
   */
  ~UUID() noexcept = default;

  /*!
   * \brief Copy constructor.
   */
  UUID(const UUID&) noexcept = default;

  /*!
   * \brief Move constructor.
   */
  UUID(UUID&&) noexcept = default;

  /*!
   * \brief Copy assignment.
   */
  UUID& operator=(const UUID&) & noexcept = default;

  /*!
   * \brief Move assignment.
   */
  UUID& operator=(UUID&&) & noexcept = default;

  /*!
   * \brief  Converts the UUID to a string with appropriate format. Note: Letters are lower case.
   * \return String representation of the UUID.
   * \trace  CREQ-158648
   */
  const UUIDString ToString() const;

  /*!
   * \brief Compares two UUIDs bytewise for equality.
   */
  bool operator==(const UUID& rhs) const;

  /*!
   * \brief Compares two UUIDs bytewise for difference.
   */
  bool operator!=(const UUID& rhs) const;
  /*!
   * \brief Compares two UUIDs bytewise for equality.
   */
  bool operator<(const UUID& rhs) const;
  /*!
   * \brief Compares two UUIDs bytewise for equality.
   */
  bool operator>(const UUID& rhs) const;
  /*!
   * \brief Returns the underlying data structure of the UUID.
   */
  const UUIDData Data() const;

 private:
  /*!
   * \brief Container for the binary data of the UUID.
   */
  std::array<uint8_t, kUuidByteLength> data_{};
};

}  // namespace id
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_ID_UUID_H_
