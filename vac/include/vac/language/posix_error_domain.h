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
/*!        \file  vac/language/posix_error_domain.h
 *        \brief  Alias for SWS core type ara::core::PosixErrorDomain.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_POSIX_ERROR_DOMAIN_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_POSIX_ERROR_DOMAIN_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "ara/core/posix_error_domain.h"

namespace vac {
namespace language {

/* VECTOR Next Construct AutosarC++17_10-M7.3.6: MD_VAC_M7.3.6_usingDeclarationInHeaderFile */
/*!
 * \copydoc ara::core::MakeErrorCode
 */
using ara::core::MakeErrorCode;

/*!
 * \brief An alias for ara::core::PosixErrc
 */
using PosixErrc = ara::core::PosixErrc;

/*!
 * \copydoc ara::core::PosixException
 */
using PosixException = ara::core::PosixException;

/*!
 * \copydoc ara::core::PosixErrorDomain
 */
using PosixErrorDomain = ara::core::PosixErrorDomain;

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_POSIX_ERROR_DOMAIN_H_
