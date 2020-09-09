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
/*!        \file  range.h
 *        \brief  Contains a Range iterator.
 *
 *      \details  This class can replace most simple for loops by using the range in combination
 *                with the range-based for loop and represents a half closed interval of integers.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_ITERATORS_RANGE_H_
#define LIB_VAC_INCLUDE_VAC_ITERATORS_RANGE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstddef>
#include <iterator>
#include <limits>
#include <stdexcept>
#include "vac/language/throw_or_terminate.h"

namespace vac {
namespace iterators {

/*!
 * \brief   Calculate end ONCE and pass the calculated value to the iterator.
 * \details If the step is only +1, then skip it and take the last element -1.
 *          Else remove the remainder of the last step from the last element.
 * \param   begin Is the start value of the range.
 * \param   end Is the exclusive end value of the range.
 * \param   step Is the step size for each iteration.
 * \return  The calculated end that will be hit via a iteration.
 * \pre     Step must not be equal to zero.
 */
template <typename I>
inline auto calculate_end(I begin, I end, I step) -> I {
  I ret_value;
  if (end == begin) {
    // Empty range.
    ret_value = begin;
  }

  if (step == 0) {
    vac::language::ThrowOrTerminate<std::invalid_argument>("Step size must not be 0!");
  }

  if (step == 1) {
    // Optimization: Standard use case: step_ is 1
    if (end < begin) {
      // Empty range.
      ret_value = begin;
    } else {
      // Nonempty range.
      ret_value = end;
    }
  } else if (step > 1) {
    if (end < begin) {
      // Empty range
      ret_value = begin;
    } else {
      I const distance{I(end - begin)};
      // Get the last step width
      I const remainder{I(distance % step)};

      if (remainder != 0) {
        // The new end that will be hit by a step
        ret_value = I(end - remainder + step);
      } else {
        ret_value = end;
      }
    }
  } else {
    // Step is smaller than 0.
    if (end > begin) {
      // Empty range.
      ret_value = begin;
    } else {
      I const distance{I(begin - end)};
      // Get the last step width.
      I const remainder{I(distance % step)};
      if (remainder == 0) {
        ret_value = end;
      } else {
        // The new end that will be hit by a step.
        ret_value = I(end + remainder + step);
      }
    }
  }
  return ret_value;
}

/*!
 * \brief   Creates a range iterator with [start, end) with an optional step.
 * \details Start will be included in the range, end will not.
 *          A range object like Pythons - can be configured with an optional step
 *          Example: for (auto n : range(1,10)) { cout << n; }.
 * \trace   CREQ-158640
 */
template <typename I = std::size_t>
class Range final {
  /*!
   * \brief Start value for the iterator.
   */
  I const begin_;
  /*!
   * \brief Step size for the iterator.
   */
  I const step_;
  /*!
   * \brief Given end value for the iterator.
   */
  I const end_;
  /*!
   * \brief Real end value for the iterator.
   */
  I const real_end_;

  static_assert(std::numeric_limits<I>::is_integer, "Type I needs to be a integer");

 public:
  /*!
   * \brief Value Type as defined in iterator interface.
   */
  using value_type = I;
  /*!
   * \brief Difference type as defined in iterator interface.
   */
  using difference_type = I;
  /*!
   * \brief Pointer type as defined in iterator interface.
   */
  using pointer = value_type *;
  /*!
   * \brief Const pointer type as defined in iterator interface.
   */
  using const_pointer = value_type const *;
  /*!
   * \brief Reference type as defined in iterator interface.
   */
  using reference = value_type &;
  /*!
   * \brief Const reference type as defined in iterator interface.
   */
  using const_reference = value_type const &;
  /*!
   * \brief Iterator category as defined in iterator interface
   *        Enables only forward iteration.
   */
  using iterator_category = std::forward_iterator_tag;

  /*!
   * \brief   Constructor from initial value, end value and step size.
   * \details The range.
   * \param   begin The initial value.
   * \param   end The end value. Defaults to the maximum of that type.
   * \param   step The step size. Defaults to 1.
   */
  Range(I begin, I end, I step) : begin_{begin}, step_{step}, end_{end}, real_end_{calculate_end(begin, end, step)} {}

  /*!
   * \brief Internal class providing the c++ iterator interface.
   *        Provides everything for a normal forward iterator, so it can be used in algorithms & range-based for.
   */
  class iterator final {
    /*!
     * \brief The real Range object.
     */
    Range const range_;
    /*!
     * \brief The value of the current iteration.
     */
    I current_;

    /*!
     * \brief Returns range iterator steps
     */
    auto step_val() const & -> I { return range_.step_; }

   public:
    /*!
     * \brief Default (& empty) constructor as defined in iterator interface.
     *        Required for: Forward Iterator.
     */
    iterator() : range_{0, 0, 1}, current_{0} {}

    /*!
     * \brief Copy constructor as defined in iterator interface.
     * \param it Original iterator to copy from.
     *        Required for: Iterator.
     */
    iterator(iterator const &it) = default;

    /*!
     * \brief  Copy assignment as defined in iterator interface.
     * \param  it Original iterator to copy from.
     * \return The copied iterator.
     *         Required for: Iterator.
     */
    auto operator=(iterator const &it) -> iterator & = default;

    /*!
     * \brief Default Destructor as defined in iterator interface.
     *        Required for: Iterator.
     */
    ~iterator() = default;

    /*!
     * \brief Constructor from Range object (const ref).
     * \param r Range object which contains all constant information.
     * \param b The start value for this iterator.
     */
    iterator(Range const &r, I b) : range_{r}, current_{b} {}
    /*!
     * \brief Constructor from Range object (move).
     * \param r Range object which contains all constant information.
     * \param b The start value for this iterator.
     */
    iterator(Range &&r, I b) : range_{r}, current_{b} {}

    /*!
     * \brief  Dereferences the iterator to its value.
     * \return The value of the current iteration by reference.
     *         Required for: Input/Output Iterator.
     */
    auto operator*() -> reference { return current_; }

    /*!
     * \brief  Dereferences the iterator to its value.
     * \return The value of the current iteration by value.
     *         Required for: Forward Iterator.
     */
    auto operator*() const -> value_type { return current_; }

    /*!
     * \brief  Dereferences the iterator to a pointer to its value.
     * \return The value of the current iteration as pointer to const.
     *         Required for: Forward Iterator.
     */
    auto operator-> () const -> const_pointer { return &current_; }

    /*!
     * \brief  Sets the iterator to its next iteration step.
     * \return A reference to the iterator.
     *         Required for: Iterator.
     */
    auto operator++() -> iterator & {
      current_ = value_type(current_ + step_val());
      return *this;
    }

    // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef
    /*!
     * \brief  Sets the iterator to its next iteration step.
     * \return A reference to the iterator.
     *         Required for: Iterator.
     */
    auto operator++(int) -> iterator {
      iterator temp{*this};
      ++(*this);
      return temp;
    }

    /*!
     * \brief  Compares two range iterators for equality.
     * \param  other The other iterator which to compare.
     * \return True if both are the same.
     *         Required for: Input/Output Iterator.
     */
    auto operator==(iterator const &other) const -> bool { return this->current_ == other.current_; }

    /*!
     * \brief  Compares two range iterators for inequality.
     * \param  other The other iterator which to compare.
     * \return False if both are the same.
     *         Required for: Input/Output Iterator.
     */
    auto operator!=(iterator const &other) const -> bool { return !(*this == other); }
  };

  /*!
   * \brief  Create iterator from start.
   * \return An range iterator object.
   */
  auto begin() -> iterator { return iterator{*this, begin_}; }

  /*!
   * \brief  Create iterator from end.
   * \return An range iterator object.
   */
  auto end() -> iterator { return iterator{*this, real_end_}; }
};

/*!
 * \brief  Creates a Range of integers which can be used as iterator.
 * \param  end_ Is the exclusive end value of the range.
 * \return A Range of integers that starts with 0 and step size 1.
 */
template <typename I>
inline auto range(I end_ = std::numeric_limits<I>::max()) -> Range<I> {
  static_assert(std::numeric_limits<I>::is_integer, "Type I needs to be an integer");

  return Range<I>{0, end_, 1};
}

/*!
 * \brief  Creates a Range of integers which can be used as iterator.
 * \param  begin_ Is the start value of the range.
 * \param  end_ Is the exclusive end value of the range.
 * \param  step_ Is the step size for each iteration. Can be set to any integer except 0. Defaults to 1.
 * \return A Range of integers.
 */
template <typename I>
inline auto range(I begin_, I end_, I step_ = 1) -> Range<I> {
  static_assert(std::numeric_limits<I>::is_integer, "Type I needs to be an integer");

  return Range<I>{begin_, end_, step_};
}

}  // namespace iterators
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_ITERATORS_RANGE_H_
