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
/*!        \file    literals.h
 *         \brief   Contains string and character literals
 *         \details This file contains literals to abstract away the encoding differences
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_UTIL_LITERALS_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_UTIL_LITERALS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <string>
#include <utility>

#include "vac/container/string_literals.h"

#include "vajson/util/types.h"

namespace vajson {
namespace writer {
inline namespace serializers {
namespace internal {

// VECTOR NL AutosarC++17_10-A7.3.6: MD_JSON_AutosarC++17_10-A7.3.6_internal_namespace
using namespace vac::container::literals;  // NOLINT(build/namespaces)

/*!
 * \brief The type for the string view that is returned
 */
using StringView = vac::container::CStringView;

/*!
 * \brief Gets a " character
 */
constexpr char8_t kQuote = '"';

/*!
 * \brief Gets a ": string
 */
constexpr StringView kQuoteAndColon = R"(":)"_sv;

/*!
 * \brief Gets a literal for 'null'
 */
constexpr StringView kNull = "null"_sv;

/*!
 * \brief Gets a literal for 'true'
 */
constexpr StringView kTrue = "true"_sv;

/*!
 * \brief Gets a literal for 'false'
 */
constexpr StringView kFalse = "false"_sv;

/*!
 * \brief Gets an escaped Quote string
 */
constexpr vac::container::CStringView kEscapedQuote = R"(\")"_sv;
/*!
 * \brief Gets a Backslash string
 */
constexpr vac::container::CStringView kBackslash = R"(\\)"_sv;
/*!
 * \brief Gets a Slash string
 */
constexpr vac::container::CStringView kSlash = R"(\/)"_sv;
/*!
 * \brief Gets a BackSpace string
 */
constexpr vac::container::CStringView kBackSpace = R"(\b)"_sv;
/*!
 * \brief Gets a FormFeed string
 */
constexpr vac::container::CStringView kFormFeed = R"(\f)"_sv;
/*!
 * \brief Gets a Newline string
 */
constexpr vac::container::CStringView kNewline = R"(\n)"_sv;
/*!
 * \brief Gets a CarriageReturn string
 */
constexpr vac::container::CStringView kCarriageReturn = R"(\r)"_sv;
/*!
 * \brief Gets a Tab string
 */
constexpr vac::container::CStringView kTab = R"(\t)"_sv;
}  // namespace internal
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_UTIL_LITERALS_H_
