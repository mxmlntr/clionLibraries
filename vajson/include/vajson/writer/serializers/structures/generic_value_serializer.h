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
/*!        \file  generic_value_serializer.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <ostream>

#include "vac/container/c_string_view.h"

#include "vajson/writer/serializers/structures/serializer.h"
#include "vajson/writer/serializers/util/escaped_json_string.h"
#include "vajson/writer/serializers/util/literals.h"
#include "vajson/writer/types/array_type.h"
#include "vajson/writer/types/basic_types.h"
#include "vajson/writer/types/object_type.h"

namespace vajson {
namespace writer {
inline namespace serializers {
/*!
 * \brief Serializes values
 * \tparam Return The return value of a << operation. If void, will return the same type again
 *
 * \vprivate
 */
template <typename Return>
class GenericValueSerializer {
 public:
  /*!
   * \brief Sets the return value to be either the same type (for arrays or a specified type) depending on Return
   */
  using Next = typename std::conditional<std::is_same<Return, Self>::value, GenericValueSerializer, Return>::type;

  /*!
   * \brief The type of the output writer
   */
  using WriterType = std::reference_wrapper<std::ostream>;

  /*!
   * \brief Constructor from an output stream
   * \param os The output stream to write into
   * \param state The serializer state - either empty or nonempty
   */
  explicit GenericValueSerializer(WriterType os, SerializerState state = SerializerState::kEmpty) noexcept
      : os_(os), serializer_state_{state} {}
  /*!
   * \brief Default move constructor
   */
  GenericValueSerializer(GenericValueSerializer&&) noexcept = default;
  /*!
   * \brief Default move assignment
   */
  auto operator=(GenericValueSerializer&&) & noexcept -> GenericValueSerializer& = default;
  /*!
   * \brief Deleted copy constructor
   */
  GenericValueSerializer(GenericValueSerializer const&) = delete;
  /*!
   * \brief Deleted copy assignment
   */
  auto operator=(GenericValueSerializer const&) & -> GenericValueSerializer& = delete;

  /*!
   * \brief Serialize a null value
   * \returns the next serializer state
   */
  auto operator<<(JNullType) const && noexcept -> Next {
    return this->Serialize([this]() noexcept { this->os_.get() << internal::kNull; });
  }

  /*!
   * \brief Serialize a boolean
   * \param b The boolean value to serialize
   * \returns the next serializer state
   */
  auto operator<<(JBoolType b) const && noexcept -> Next {
    vac::container::CStringView const value{b.value ? internal::kTrue : internal::kFalse};
    return this->Serialize([ this, value ]() noexcept { this->os_.get() << value; });
  }

  /*!
   * \brief Serialize a number
   * \tparam T the actual type of number
   * \param num the number value to serialize
   * \returns the next serializer state
   */
  template <typename T>
      auto operator<<(JNumberType<T> num) const && noexcept -> Next {
    return this->Serialize([ this, num ]() noexcept { this->os_.get() << num.GetValue(); });
  }

  /*!
   * \brief Serialize a string
   * \param s the string value to serialize
   * \returns the next serializer state
   */
  auto operator<<(JStringType s) const && noexcept -> Next {
    return this->Serialize([ this, s ]() noexcept {
      this->os_.get() << internal::kQuote << internal::EscapedJsonString(s) << internal::kQuote;
    });
  }

  /*!
   * \brief Serialize a series of serializable values
   * \tparam Fn The function defining how to serialize
   * \param tup the tuple to serialize
   * \returns the next serializer state
   */
  template <typename Fn>
  auto operator<<(JArrayType<Fn> tup) const&& noexcept(false) -> Next {
    return this->Serialize([this, tup]() {
      this->os_.get().put('[');
      tup.fn(ArrayStart(this->os_));
      this->os_.get().put(']');
    });
  }

  /*!
   * \brief Serialize an object
   * \tparam Fn The function defining how to serialize
   * \param obj The object to serialize
   * \returns the next serializer state
   */
  template <typename Fn>
  auto operator<<(JObjectType<Fn> obj) const&& noexcept(false) -> Next {
    return this->Serialize([this, &obj]() {
      using ReturnType = decltype(obj.fn(ObjectStart(this->os_)));
      constexpr static bool kIsEmpty{std::is_same<ReturnType, KeySerializer>::value};
      constexpr static bool kIsValid{std::is_same<ReturnType, KeySerializer>::value};
      static_assert(kIsEmpty || kIsValid, "Cannot close object after key");

      this->os_.get().put('{');
      static_cast<void>(obj.fn(ObjectStart(this->os_)));
      this->os_.get().put('}');
    });
  }

 private:
  /*!
   * \brief Actually serializes the values
   * \tparam Fn The type of the function to call
   * \param fn the serializer call function
   * \return another tuple serializer
   */
  template <typename Fn>
  auto Serialize(Fn&& fn) const noexcept(noexcept(std::declval<Fn>()())) -> Next {
    if (this->serializer_state_ == SerializerState::kNonEmpty) {
      this->os_.get() << ',';
    }
    fn();
    return Next(this->os_, SerializerState::kNonEmpty);
  }

  /*!
   * \brief The output stream into which to write in
   */
  WriterType os_;
  /*!
   * \brief The serializer state
   * \details Checks if the current object is empty to append commas correctly
   */
  SerializerState serializer_state_;
};
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_H_
