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
/*!        \file  generated_memory_config.h
 *        \brief  Compile time memory configuration for libvac.
 *
 *      \details  THIS FILE IS INTENDED FOR TEMPORARY USAGE ONLY. In the future the configuration will be generated.
 *                When a concept is available for compile-time config of libvac naming might change as well.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_GENERATED_MEMORY_CONFIG_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_GENERATED_MEMORY_CONFIG_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

namespace vac {
namespace memory {

/*!
 * \brief Compile time constant for differentiating between deterministic and non-deterministic mode.
 */
constexpr const bool kIsDeterministicMode{false};

}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_GENERATED_MEMORY_CONFIG_H_
