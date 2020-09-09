/**********************************************************************************************************************
 *  COPYRIGHT
 * -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2019 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 * -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 * -----------------------------------------------------------------------------------------------------------------*/
/*!       \file     types.h
 *        \brief    TODO
 *
 *        \details  TODO
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_UTIL_TYPES_H_
#define LIB_VAJSON_INCLUDE_VAJSON_UTIL_TYPES_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

namespace vajson {
/*!
 * \brief Abstraction for char type
 *
 * \vprivate
 */
using char8_t = char;

/*!
 * \brief Abstraction for return values from posix functions
 *
 * \vprivate
 */
using PosixInteger = int;  // NOLINT(runtime/int)
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_UTIL_TYPES_H_
