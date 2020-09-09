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
/*!       \file     parser_state.h
 *        \brief    Contains definition for the parser state
 *
 *        \details  -
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_PARSER_STATE_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_PARSER_STATE_H_

namespace vajson {
inline namespace reader {
/*!
 * \brief State that is used for every parser event
 *
 * \vpublic
 */
enum class ParserState : bool { kRunning, kFinished };
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_PARSER_STATE_H_
