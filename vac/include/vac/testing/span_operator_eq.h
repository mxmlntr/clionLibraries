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
/*!        \file  span_operator_eq.h
 *        \brief  Provides equality operator for ara::core::Span for gmock matchers.
 *
 *      \details  Implementation for bool operator==(const ara::core::Span, const ara::core::Span).
 *                The function is made available in testing internal namespace, so that gmock matchers
 *                will have access to it, while production code outside that namespace will not.
 *
 *                It is used for modules with tests heavily dependent on vac::container::array_view
 *                being comparable. Migration to ara::core::Span without comparison operators was problematic
 *                for these modules, This file is only used for these modules to alleviate migration headache.
 *                DO NOT use this file for new tests.
 *
 *                This file must be included BEFORE gmock.h when compiling with clang.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_TESTING_SPAN_OPERATOR_EQ_H_
#define LIB_VAC_INCLUDE_VAC_TESTING_SPAN_OPERATOR_EQ_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include "ara/core/span.h"

namespace testing {
namespace internal {

/*! \brief Equality operator for ara::core::Span. Should only be used in tests. */
template <typename T, typename U>
bool operator==(const ara::core::Span<T> lhs, const ara::core::Span<U> rhs) {
  return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

/*! \brief Inequality operator for ara::core::Span. Should only be used in tests. */
template <typename T, typename U>
bool operator!=(const ara::core::Span<T> lhs, const ara::core::Span<U> rhs) {
  return !(lhs == rhs);
}

}  // namespace internal
}  // namespace testing

#endif  // LIB_VAC_INCLUDE_VAC_TESTING_SPAN_OPERATOR_EQ_H_
