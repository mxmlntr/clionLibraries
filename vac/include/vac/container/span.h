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
/*!        \file  vac/container/span.h
 *        \brief  Alias for SWS core type ara::core::Span.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_SPAN_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_SPAN_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "ara/core/span.h"

namespace vac {
namespace container {

/*!
 * \brief  A view over a contiguous sequence of objects.
 * \tparam T Type of the elements in Span.
 * \tparam Extent Extent of Span.
 */
template <typename T, std::size_t Extent = ara::core::dynamic_extent>
using span = ara::core::Span<T, Extent>;

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_SPAN_H_
