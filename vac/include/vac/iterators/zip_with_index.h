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
/*!        \file  zip_with_index.h
 *        \brief  Contains an iterator that zips the passed iterator with an index.
 *
 *      \details  This class can replace most simple for loops by using ZipWithIndex on an container in combination
 *                with the range-based for loop.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_ITERATORS_ZIP_WITH_INDEX_H_
#define LIB_VAC_INCLUDE_VAC_ITERATORS_ZIP_WITH_INDEX_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <utility>

#include "vac/language/cpp14_backport.h"

namespace vac {
namespace iterators {

/*!
 * \brief Value type of the Zipped Iterator.
 */
template <typename T, typename I>
struct ZipWithIndexValue final {
  /*!
   * \brief Constructor from value and index.
   * \param v The value for this iteration.
   * \param i The index for this iteration.
   */
  ZipWithIndexValue(T v, I i) : value(v), index(i) {}
  /*!
   * \brief Contains the value of the current iteration.
   */
  T value;

  /*!
   * \brief Contains the index of the current iteration.
   */
  I index;
};

/*!
 * \brief  Zipped iterable that is able to construct the index iterator.
 * \tparam Iterable The contained iterable type.
 * \trace  CREQ-158639
 */
template <typename Iterable>
class ZipWithIndex final {
  /*!
   * \brief Value iterable type.
   */
  using iterable_t = typename std::remove_reference<Iterable>::type;

  /*!
   * \brief Value type of the contained iterable.
   */
  using iterable_value_type = typename iterable_t::value_type;

 public:
  /*!
   * \brief The size type for this iterable.
   */
  using size_type = typename iterable_t::size_type;
  /*!
   * \brief The difference type for this iterable.
   */
  using difference_type = typename iterable_t::difference_type;

  /*!
   * \brief Value Type as defined in iterator interface.
   */
  using value_type = ZipWithIndexValue<iterable_value_type, size_type>;
  /*!
   * \brief Type of the values but const-qualified.
   */
  using const_value_type = ZipWithIndexValue<iterable_value_type const, size_type>;

  /*!
   * \brief Reference Type as defined in iterator interface.
   */
  using reference = ZipWithIndexValue<iterable_value_type&, size_type>;

  /*!
   * \brief Const reference Type as defined in iterator interface.
   */
  using const_reference = ZipWithIndexValue<iterable_value_type const&, size_type>;

  /*!
   * \brief Iterator Category as defined in iterator interface
   *        Enables only forward iteration.
   */
  using iterator_category = std::forward_iterator_tag;

  /*!
   * \brief Constructor taking iterable, initial offset and constant step size!.
   * \param iterable The iterable that is to be zipped.
   * \param offset The offset from which the index shall start.
   * \param step The step size that is added after each step.
   */
  ZipWithIndex(iterable_t&& iterable, size_type offset, difference_type step)
      : iterable_{std::move(iterable)}, offset_{offset}, step_size_{step} {}

  /*!
   * \brief Constructor taking iterable, initial offset and constant step size!.
   * \param iterable The iterable that is to be zipped.
   * \param offset The offset from which the index shall start.
   * \param step The step size that is added after each step.
   */
  ZipWithIndex(iterable_t const& iterable, size_type offset, difference_type step)
      : iterable_{iterable}, offset_{offset}, step_size_{step} {}

  /*!
   * \brief Inner class that will create the iterator.
   */
  template <typename Value>
  class Iterator final {
   public:
    /*!
     * \brief Value iterator type.
     */
    using iterator_t = decltype(std::begin(std::declval<Iterable>()));

    /*!
     * \brief The type of value from the value-iterator.
     */
    using iterator_value_t = decltype(*(std::declval<iterator_t>()));

    /*!
     * \brief Copy Constructor as defined in iterator interface.
     * \param it Original iterator to copy from. Required for: Iterator.
     */
    Iterator(Iterator const& it) = default;

    /*!
     * \brief Constructor that will initialize from offset.
     * \param it The iterator that is to be zipped.
     * \param zip_object Contains information on the offset and stepsize for the index.
     */
    Iterator(iterator_t&& it, ZipWithIndex const& zip_object)
        : value_iterator_{std::forward<iterator_t>(it)},
          index_{zip_object.offset_},
          step_size_{zip_object.step_size_} {}

    /*!
     * \brief  Copy Assignment as defined in iterator interface.
     * \param  it Original iterator to copy from.
     * \return The copied iterator. Required for: Iterator.
     */
    auto operator=(Iterator const& it) & -> Iterator& = default;

    /*!
     * \brief Default Destructor as defined in iterator interface. Required for: Iterator.
     */
    ~Iterator() noexcept = default;

    /*!
     * \brief  Gets the value zipped with the index.
     * \return The value zipped with the index.
     */
    auto operator*() -> ZipWithIndexValue<iterator_value_t, size_type> {
      return ZipWithIndexValue<iterator_value_t, size_type>(*(this->value_iterator_), this->index_);
    }

    /*!
     * \brief  Sets the iterator to its next iteration step.
     * \return A reference to the modified iterator. Required for: Iterator.
     */
    auto operator++() -> Iterator& {
      this->index_ += this->step_size_;
      ++this->value_iterator_;
      return *this;
    }

    // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef
    /*!
     * \brief  Sets the iterator to its next iteration step.
     * \return A reference to an unmodified iterator. Required for: Iterator.
     */
    auto operator++(int) -> Iterator {
      Iterator tmp{*this};
      ++(*this);
      return tmp;
    }

    /*!
     * \brief  Forwards check for equality towards value iterator (the index is non-ending!).
     * \param  other The other iterator which to compare.
     * \return True if both are the same. Required for: Input/Output Iterator.
     */
    auto operator==(Iterator const& other) const -> bool { return (this->value_iterator_ == other.value_iterator_); }

    /*!
     * \brief  Forwards check for inequality towards value iterator (the index is non-ending!).
     * \param  other The other iterator which to compare.
     * \return True if both are different. Required for: Input/Output Iterator.
     */
    auto operator!=(Iterator const& other) const -> bool { return !operator==(other); }

   private:
    /*!
     * \brief Value iterator in current iteration.
     */
    iterator_t value_iterator_;
    /*!
     * \brief Index of the current iteration.
     */
    size_type index_;
    /*!
     * \brief Step size for each step, can be any integral value.
     */
    difference_type step_size_;
  };

  /*!
   * \brief Iterator type for mutable iteration.
   */
  using iterator = Iterator<iterable_value_type>;
  /*!
   * \brief Iterator type for constant iteration.
   */
  using const_iterator = Iterator<typename std::remove_const<iterable_value_type>::type const>;

  /*!
   * \brief   Create iterator from start.
   * \details We use the passed iterable to create a iterator from start.
   * \return  An ZipWithIndex iterator object.
   */
  auto begin() -> iterator { return {std::begin(this->iterable_), *this}; }
  /*!
   * \brief   Create iterator from the sentinel value.
   * \details We use the passed iterable to create a iterator with the end sentinel value.
   * \return  An ZipWithIndex iterator object.
   */
  auto end() -> iterator { return {std::end(this->iterable_), *this}; }

  /*!
   * \brief   Create iterator from start.
   * \details We use the passed iterable to create a iterator from start.
   * \return  An ZipWithIndex iterator object.
   */
  auto begin() const -> const_iterator { return {std::begin(this->iterable_), *this}; }
  /*!
   * \brief   Create iterator from the sentinel value.
   * \details We use the passed iterable to create a iterator with the end sentinel value.
   * \return  An ZipWithIndex iterator object.
   */
  auto end() const -> const_iterator { return {std::end(this->iterable_), *this}; }

  /*!
   * \brief   Create iterator from start.
   * \details We use the passed iterable to create a iterator from start.
   * \return  An ZipWithIndex iterator object.
   */
  auto cbegin() const -> const_iterator { return this->begin(); }
  /*!
   * \brief   Create iterator from the sentinel value.
   * \details We use the passed iterable to create a iterator with the end sentinel value.
   * \return  An ZipWithIndex iterator object.
   */
  auto cend() const -> const_iterator { return this->end(); }

 private:
  /*!
   * \brief Value iterator in first iteration.
   */
  iterable_t iterable_;
  /*!
   * \brief Initial index of the iterator.
   */
  size_type offset_;
  /*!
   * \brief Step size for each step, can be any integral value.
   */
  difference_type step_size_;
};

/*!
 * \brief   Creates an iterable which returns an item which contains the current index & value of the iterator.
 * \details Can be customized with initial offset and a stepsize (which may be 0 or negative).
 * \tparam  Iterable The iterable type to be zipped.
 * \param   iterable The iterable that is to be zipped.
 * \param   offset The offset from which the index shall start.
 * \param   step The step size that is added after each step.
 * \return  The iterable zipped with the specified index.
 */
template <typename Iterable>
auto zip_with_index(Iterable&& iterable, typename language::decay_t<Iterable>::size_type offset = 0,
                    typename language::decay_t<Iterable>::difference_type step = 1) -> ZipWithIndex<Iterable> {
  return ZipWithIndex<Iterable>(std::forward<Iterable>(iterable), offset, step);
}

}  // namespace iterators
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_ITERATORS_ZIP_WITH_INDEX_H_
