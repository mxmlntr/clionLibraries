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
/*!        \file  ara/core/abort.h
 *        \brief  Explicit abnormal process termination.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_ABORT_H_
#define LIB_VAC_INCLUDE_ARA_CORE_ABORT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

namespace ara {
namespace core {

/*!
 * \brief   Terminate the current process abnormally.
 * \details Before terminating, a log message with FATAL severity is being output, which includes the text given as
 *          argument to this function.
 *          This function will never return to its caller. The stack is not unwound: destructors of variables with
 *          automatic storage duration are not called.
 *          The text argument is expected to point to a null-terminated string with static storage duration.
 * \note    The type of the text argument is a raw pointer (instead of a more "modern" type such as StringView) in order
 *          to increase the chances that the function call succeeds even in situations when e.g. the stack has been
 *          corrupted.
 * \param   text A custom text to include in the log message being output.
 * \vprivate
 */
[[noreturn]] void Abort(char const* text);

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_ABORT_H_
