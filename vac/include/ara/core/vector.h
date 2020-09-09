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
/*!        \file  ara/core/vector.h
 *        \brief  SWS core type ara::core::Vector.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_VECTOR_H_
#define LIB_VAC_INCLUDE_ARA_CORE_VECTOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <vector>
#include "vac/memory/phase_managed_allocator.h"

namespace ara {
namespace core {

/*!
 * \brief  Container which can change in size.
 * \tparam T Type of the elements in Vector.
 * \tparam Alloc Allocator of Vector.
 * \trace  CREQ-199567
 * \vpublic
 */
template <typename T, typename Alloc = vac::memory::PhaseManagedAllocator<T>>
using Vector = std::vector<T, Alloc>;

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_VECTOR_H_
