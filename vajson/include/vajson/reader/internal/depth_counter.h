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
/**     \file       depth_counter.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_DEPTH_COUNTER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_DEPTH_COUNTER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdint>
#include <utility>

#include "ara/core/vector.h"

#include "vajson/reader/internal/item_stack.h"
#include "vajson/util/json_error_domain.h"

namespace vajson {
inline namespace reader {
namespace internal {

/*!
 * \brief Tracks the depth of the parser
 */
class DepthCounter final {
 public:
  // VECTOR NC AutosarC++17_10-A15.4.5: MD_JSON_AutosarC++17_10-A15.4.5_static_vector
  /*!
   * \brief Initializes the depth counter for the JSON document
   * \param depth The maximum depth of the JSON document
   */
  explicit DepthCounter(std::size_t depth) noexcept : counter_{} { counter_.reserve(depth); }

  /*!
   * \brief Default move constructor
   */
  DepthCounter(DepthCounter&&) = default;  // VECTOR SL AutosarC++17_10-A15.5.1: MD_JSON_false_noexcept
  /*!
   * \brief Disabled copy constructor
   */
  DepthCounter(DepthCounter const&) = delete;
  /*!
   * \brief Default move assignment
   * \return A reference to the moved into object
   */
  DepthCounter& operator=(DepthCounter&&) & = default;  // VECTOR SL AutosarC++17_10-A15.5.1: MD_JSON_false_noexcept
  /*!
   * \brief Deleted copy assignment
   * \return A reference to the copied into object
   */
  DepthCounter& operator=(DepthCounter const&) & = delete;

  /*!
   * \brief Checks if a eof is correct
   * \return a positive result if the stack is empty
   * \error{JsonErrorDomain, JsonErrc::kExpectedClosingBrackets, if ']' was expected}
   * \error{JsonErrorDomain, JsonErrc::kExpectedClosingBraces, if '}' was expected}
   */
  Result<void> CheckEndOfFile() const noexcept {
    return MakeResult(this->IsEmpty(), [this]() {
      return MakeErrorCode((this->GetActiveItem() == Type::kArray) ? JsonErrc::kExpectedClosingBrackets
                                                                   : JsonErrc::kExpectedClosingBraces,
                           "DepthCounter::CheckEndOfFile");
    });
  }

  /*!
   * \brief Checks if the Depth is not zero
   * \returns a positive result if the stack is nonempty
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOnTopLevel, if not in an object or array}
   */
  Result<void> CheckNonEmpty() const noexcept {
    return MakeResult(not this->IsEmpty(), JsonErrc::kUnexpectedOnTopLevel, "DepthCounter::CheckNonEmpty");
  }

  /*!
   * \brief Add Array to stack
   * \returns a positive result if an array can be started
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBrackets, No opening brackets were expected}
   */
  Result<void> AddArray() noexcept {
    bool added_array{this->Push(ItemStack::Array())};
    return MakeResult(added_array, JsonErrc::kUnexpectedOpeningBrackets, "DepthCounter::AddArray");
  }

  /*!
   * \brief Add Object to stack
   * \returns a positive result if an object can be started
   * \error{JsonErrorDomain, JsonErrc::kUnexpectedOpeningBraces, No opening braces were expected}
   */
  Result<void> AddObject() noexcept {
    bool added_object{this->Push(ItemStack::Object())};
    return MakeResult(added_object, JsonErrc::kUnexpectedOpeningBraces, "DepthCounter::AddObject");
  }

  /*!
   * \brief Pop Object from Stack
   * \returns a positive result if the end object call was valid
   * \error{JsonErrorDomain, JsonErrc::kExpectedValue, If a value is expected}
   * \error{JsonErrorDomain, JsonErrc::kNotInObject, If in an array or on toplevel}
   */
  Result<std::size_t> PopObject() noexcept {
    bool expected_key{TryExecuteOnLast([](ItemStack const& is) { return is.ExpectKey(); })};
    return MakeResult(expected_key, JsonErrc::kExpectedValue, "DepthCounter::PopObject")
        .AndThen([this]() { return this->CheckObjectActive(); })
        .Map([this]() {
          std::size_t count = this->CurrentCount();
          this->Pop();
          return count;
        });
  }

  /*!
   * \brief Pop Array from Stack
   * \returns a positive result if the end array call was valid
   * \error{JsonErrorDomain, JsonErrc::kNotInArray, if in an object or on toplevel}
   */
  Result<std::size_t> PopArray() noexcept {
    return this->CheckArrayActive().Map([this]() {
      std::size_t count = this->CurrentCount();
      this->Pop();
      return count;
    });
  }

  /*!
   * \brief Adds a single key
   * \returns a positive result if adding a value is working correctly.
   * \error{JsonErrorDomain, JsonErrc::kExpectedValue, if a value was expected}
   */
  Result<void> AddKey() noexcept {
    bool result{TryExecuteOnLast([](ItemStack& is) { return is.AddKey(); })};
    return MakeResult(result, JsonErrc::kExpectedValue, "DepthCounter::AddKey");
  }

  /*!
   * \brief Adds a single value
   * \returns a positive result if adding a value is working correctly.
   * \error{JsonErrorDomain, JsonErrc::kExpectedKey, if a key was expected}
   */
  Result<void> AddValue() noexcept {
    Result<void> result{};
    // Check if in simple value or object/array
    if (!this->IsEmpty()) {
      // inside object or array
      bool expected_value = this->GetActiveItem().AddValue();
      result = MakeResult(expected_value, JsonErrc::kExpectedKey, "DepthCounter::AddValue");
    }

    return result;
  }

 private:
  /*!
   * \brief Returns the active state
   * \returns the active state
   * \details Calling on empty counter invokes undefined behaviour
   * \throws std::out_of_range If the queue is empty
   */
  ItemStack& GetActiveItem() noexcept(false) { return this->counter_.back(); }

  /*!
   * \brief Returns the active state
   * \returns the active state
   * \details Calling on empty counter invokes undefined behaviour
   * \throws std::out_of_range If the queue is empty
   */
  ItemStack const& GetActiveItem() const noexcept(false) { return this->counter_.back(); }

  /*!
   * \brief Gets the current count of the current item
   */
  std::size_t CurrentCount() const { return this->GetActiveItem().Count(); }

  /*!
   * \brief Check if the max Depth is reached
   */
  bool IsFull() const noexcept { return this->Depth() >= this->counter_.capacity(); }

  /*!
   * \brief Checks if the Depth is 0
   */
  bool IsEmpty() const noexcept { return this->counter_.empty(); }

  /*!
   * \brief Gets the depth of the Counter
   */
  std::size_t Depth() const noexcept { return this->counter_.size(); }

  /*!
   * \brief Adds an item to the stack
   */
  bool Push(ItemStack item) noexcept {
    bool is_not_full{!this->IsFull()};
    if (is_not_full) {
      counter_.push_back(item);
    }
    return is_not_full;
  }

  /*!
   * \brief Pops Item from Stack
   */
  void Pop() noexcept {
    assert(not this->IsEmpty());
    this->counter_.pop_back();
  }

  /*!
   * \brief Checks if the parser is in an object
   * \returns a positive result if an object is active
   * \error{JsonErrorDomain, JsonErrc::kNotInObject, if in an array or on toplevel}
   */
  Result<void> CheckObjectActive() const noexcept {
    bool condition = not(this->IsEmpty() && (this->GetActiveItem() != Type::kObject));
    return MakeResult(condition, JsonErrc::kNotInObject, "DepthCounter::CheckObjectActive");
  }

  /*!
   * \brief Checks if the parser is in an array
   * \returns a positive result if an array is active
   * \error{JsonErrorDomain, JsonErrc::kNotInArray, if in an object or on toplevel}
   */
  Result<void> CheckArrayActive() const noexcept {
    bool condition = not(this->IsEmpty() && (this->GetActiveItem() != Type::kArray));
    return MakeResult(condition, JsonErrc::kNotInArray, "DepthCounter::CheckArrayActive");
  }

  /*!
   * \brief Tries to execute the lambda on top of the stack
   * \tparam Fn The function to call
   * \param fn The function to call
   * \return true if execution was successful
   */
  template <typename Fn>
  bool TryExecuteOnLast(Fn&& fn) noexcept {
    bool is_valid{false};
    if (not this->IsEmpty()) {
      ItemStack& is = this->GetActiveItem();
      is_valid = std::forward<Fn>(fn)(is);
    }
    return is_valid;
  }

  /*!
   * \brief Storage for the depth
   * Todo: Replace with stack allocated object
   */
  ara::core::Vector<ItemStack> counter_;
};
}  // namespace internal
}  // namespace reader
}  // namespace vajson
#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_DEPTH_COUNTER_H_
