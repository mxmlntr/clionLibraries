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
/**        \file  json_reader_cfg.h
 *        \brief  Configuration data of vajson
 *      \details  Contains application specific configuration
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_CONFIG_JSON_READER_CFG_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_CONFIG_JSON_READER_CFG_H_

#include <string>

namespace vajson {
inline namespace reader {
namespace internal {
namespace config {
/*!
 * \brief Contains the key buffer size
 */
constexpr static std::size_t kKeyBufferSize{1024};

/*!
 * \brief Contains the string buffer size
 */
constexpr static std::size_t kStringBufferSize{1024};

/*!
 * \brief Contains the buffer size
 */
constexpr static std::streamsize kBufferSize{4096};

/*!
 * \brief Depth maximum limit
 */
constexpr static std::size_t kMaxDepth{0x20};
}  // namespace config
}  // namespace internal
}  // namespace reader
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_CONFIG_JSON_READER_CFG_H_
