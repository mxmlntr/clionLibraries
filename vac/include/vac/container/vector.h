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
/*!        \file  vac/container/vector.h
 *        \brief  Alias for SWS core type ara::core::Vector.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_VECTOR_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_VECTOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "ara/core/vector.h"

namespace vac {
namespace container {

/*!
 * \brief  Class to manage a contiguous memory segment.
 * \tparam T Type of the elements in Vector.
 * \tparam Alloc Allocator of Vector.
 */
template <typename T, typename Alloc = vac::memory::PhaseManagedAllocator<T>>
using Vector = ara::core::Vector<T, Alloc>;

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_VECTOR_H_
