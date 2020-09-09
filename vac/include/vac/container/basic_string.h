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
/*!        \file  basic_string.h
 *        \brief  Implementation of the basic_string container.
 *
 *      \details  The container is based on std::string and includes AUTOSAR R19-03 extensions.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_BASIC_STRING_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_BASIC_STRING_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <string>
#include <utility>

#include "ara/core/string.h"
#include "vac/memory/generated_memory_config.h"
#include "vac/memory/phase_managed_allocator.h"
#include "vac/memory/three_phase_allocator.h"

namespace vac {
namespace container {

/*!
 * \brief An alias for ara::core::ManagedString.
 */
using String = ara::core::ManagedString;

/*!
 * \brief An alias for ara::core::basic_string<>.
 */
template <typename character_type, class Traits, class Allocator>
using basic_string = ara::core::basic_string<character_type, Traits, Allocator>;

/*!
 * \brief An alias for ara::core::SafeString.
 */
using SafeString = ara::core::SafeString;

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_BASIC_STRING_H_
