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
/*!        \file  key_serializer.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "vajson/writer/serializers/structures/serializer.h"
#include "vajson/writer/serializers/util/escaped_json_string.h"

#include "vajson/util/types.h"
#include "vajson/writer/types/array_type.h"
#include "vajson/writer/types/basic_types.h"
#include "vajson/writer/types/object_type.h"

namespace vajson {
namespace writer {
inline namespace serializers {
/*!
 * \brief This class allows only for adding a key into the object
 * \details the next concatenation will allow only a value
 *
 * \vprivate
 */
class KeySerializer final {
 public:
  /*!
   * \brief the serializer state after adding the key
   */
  using Next = ObjectSerializerValue;

  /*!
   * \brief Constructor from an output stream
   * \param os the output stream to write into
   * \param state The serializer state - either empty or nonempty
   */
  explicit KeySerializer(std::reference_wrapper<std::ostream> os,
                         SerializerState state = SerializerState::kEmpty) noexcept
      : os_(os), serializer_state_{state} {}
  /*!
   * \brief Default move constructor
   */
  KeySerializer(KeySerializer&&) noexcept = default;
  /*!
   * \brief Default move assignment
   */
  auto operator=(KeySerializer&&) & noexcept -> KeySerializer& = default;

  /*!
   * \brief Deleted copy constructor
   */
  KeySerializer(KeySerializer const&) = delete;
  /*!
   * \brief Deleted copy assignment
   */
  auto operator=(KeySerializer const&) & -> KeySerializer& = delete;

  /*!
   * \brief Serialize a key
   * \param k The key to serialize
   * \returns a value serializer
   */
  auto operator<<(JKeyType k) const && noexcept -> Next {
    if (this->serializer_state_ == SerializerState::kNonEmpty) {
      this->os_.get().put(',');
    }

    this->os_.get() << internal::kQuote << internal::EscapedJsonString(k) << internal::kQuoteAndColon;

    return Next(this->os_);
  }

 private:
  /*!
   * \brief The output stream into which to write in
   */
  std::reference_wrapper<std::ostream> os_;

  /*!
   * \brief The serializer state
   * \details Checks if the current object is empty to append commas correctly
   */
  SerializerState serializer_state_;
};
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H_
