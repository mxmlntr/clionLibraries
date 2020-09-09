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
/*!        \file  reverse.h
 *        \brief  Contains a Reversal iterator.
 *
 *      \details  This class enables backward iteration on compatible classes.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_ITERATORS_REVERSE_H_
#define LIB_VAC_INCLUDE_VAC_ITERATORS_REVERSE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstddef>
#include <iterator>
#include <limits>
#include <stdexcept>

namespace vac {
namespace iterators {

/*!
 * \brief Type that reverses a bidirectional iterator for use in a range based for loop.
 */
template <typename Iterable>
class Reverse final {
 public:
  /*!
   * \brief Constructor from an iterator.
   */
  explicit Reverse(Iterable& it) : iterable_{it} {}

  /*!
   * \brief  Gets an iterator to the end of the passed iterable.
   * \return An iterator to the end of the passed iterable.
   */
  typename Iterable::reverse_iterator begin() { return iterable_.rbegin(); }
  /*!
   * \brief  Gets an iterator beyond the start of the passed iterable.
   * \return An iterator beyond the start of the passed iterable.
   */
  typename Iterable::reverse_iterator end() { return iterable_.rend(); }

 private:
  /*!
   * \brief The iterable to be reversed.
   */
  Iterable& iterable_;
};

/*!
 * \brief Type that reverses a bidirectional iterator for use in a range based for loop.
 */
template <typename Iterable>
class ConstReverse final {
 public:
  /*!
   * \brief Constructor from an iterator.
   */
  explicit ConstReverse(Iterable const& it) : iterable_{it} {}

  /*!
   * \brief  Gets an iterator to the end of the passed iterable.
   * \return An iterator to the end of the passed iterable.
   */
  typename Iterable::const_reverse_iterator begin() const { return iterable_.rbegin(); }
  /*!
   * \brief  Gets an iterator beyond the start of the passed iterable.
   * \return An iterator beyond the start of the passed iterable.
   */
  typename Iterable::const_reverse_iterator end() const { return iterable_.rend(); }

 private:
  /*!
   * \brief The iterable to be reversed.
   */
  Iterable const& iterable_;
};

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief  Helper function to reverse an iterable.
 * \param  it The iterable to reverse.
 * \return The reversed iterable.
 */
template <typename Iterable>
auto reverse(Iterable& it) -> Reverse<Iterable> {
  return Reverse<Iterable>{it};
}

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief  Helper function to reverse a const iterable.
 * \param  it The const iterable to reverse.
 * \return The const reversed iterable.
 */
template <typename Iterable>
auto reverse(Iterable const& it) -> ConstReverse<Iterable> {
  return ConstReverse<Iterable>{it};
}
}  // namespace iterators
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_ITERATORS_REVERSE_H_
