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
/*!        \file  amsr/generic/exit_code.h
 *        \brief  Exit code enum for corruption errors.
 *
 *      \details  Globally unique exit codes to be used when exiting due to a corruption error. The values in this enum
 *                is limited to the range 2..127.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_AMSR_GENERIC_EXIT_CODE_H_
#define LIB_VAC_INCLUDE_AMSR_GENERIC_EXIT_CODE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdint>

namespace amsr {
namespace generic {

/*!
 * \brief Exit code enum.
 * \trace CREQ-LibVac-ExitCode001
 * \vpublic
 */
enum class ExitCode : std::uint8_t {
  config_parse_error = 2,       /*!< \brief Config parse error. */
  out_of_memory = 3,            /*!< \brief Out of memory. */
  insufficient_permissions = 4, /*!< \brief Insufficient permissions. */
};

}  // namespace generic
}  // namespace amsr

#endif  // LIB_VAC_INCLUDE_AMSR_GENERIC_EXIT_CODE_H_
