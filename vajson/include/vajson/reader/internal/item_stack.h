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
/**     \file       item_stack.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_ITEM_STACK_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_ITEM_STACK_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdint>

#include "vajson/util/types.h"

namespace vajson {
inline namespace reader {
class JsonOps;
namespace internal {

/*!
 * \brief Type of expectation for next item
 */
enum class Expectation : bool { kKey, kValue };

/*!
 * \brief Type of depth (Array, Object)
 */
enum class Type : bool { kObject, kArray };

/*!
 * \brief Contains info on which type is contained and how many events have been parsed
 */
class ItemStack final {
 public:
  /*!
   * \brief Make an ItemStack for an Array
   */
  static ItemStack Array() noexcept { return ItemStack{Type::kArray, Expectation::kValue}; }

  /*!
   * \brief Make an ItemStack for an Object
   */
  static ItemStack Object() noexcept { return ItemStack{Type::kObject, Expectation::kKey}; }

  /*!
   * \brief Enable Comparisons between ItemStack and its type
   */
  bool operator==(Type const& type) const noexcept { return this->type_ == type; }

  /*!
   * \brief Enable Comparisons between ItemStack and its type
   */
  bool operator!=(Type const& type) const noexcept { return this->type_ != type; }

  /*!
   * \brief Gets the current Count
   */
  std::size_t Count() const noexcept { return this->count_; }

  /*!
   * \brief Adds another value to the current item
   */
  bool AddValue() noexcept {
    this->Increment();
    bool result{true};
    if (this->type_ == Type::kObject) {
      result = this->ExpectValue();
      this->SetExpectation(Expectation::kKey);
    }
    return result;
  }

  /*!
   * \brief Adds another key to the current item
   */
  bool AddKey() noexcept {
    bool result{this->ExpectKey()};
    this->SetExpectation(Expectation::kValue);
    return result;
  }

  /*!
   * \brief Check if Key is expected
   */
  bool ExpectKey() const noexcept { return this->CheckExpectation(Expectation::kKey); }

  /*!
   * \brief Check if a Value is expected
   */
  bool ExpectValue() const noexcept { return this->CheckExpectation(Expectation::kValue); }

 private:
  /*!
   * \brief Private Constructor from a type
   */
  ItemStack(Type type, Expectation ex) noexcept : count_{0}, type_{type}, expectation_{ex} {}

  /*!
   * \brief What to expect next
   */
  void SetExpectation(Expectation ex) noexcept { this->expectation_ = ex; }
  /*!
   * \brief Checks if Expectations are correct
   */
  bool CheckExpectation(Expectation ex) const noexcept { return this->expectation_ == ex; }

  /*!
   * \brief Increments the current count
   */
  void Increment() noexcept { this->count_ += 1; }

  /*!
   * \brief The number of events
   */
  std::size_t count_;
  /*!
   * \brief The contained type
   */
  Type type_;
  /*!
   * \brief Next expected type
   */
  Expectation expectation_;
};
}  // namespace internal
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_ITEM_STACK_H_
