/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2020 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/**     \file       level_validator.h
 *      \brief
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_LEVEL_VALIDATOR_H_
#define LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_LEVEL_VALIDATOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "vajson/reader/parser.h"
#include "vajson/util/json_error_domain.h"

namespace vajson {
inline namespace reader {
namespace internal {

/*!
 * \brief Unqualified access to vajson types
 */
using ParserResult = Result<ParserState>;

/*!
 * \brief Semi-automatic checks for single object parsers
 */
class LevelValidator {
 public:
  /*!
   * \brief Try to enter an object/array
   * \return Running in case no object/array has been entered yet
   * \error{JsonErrorDomain, JsonErrc::user_validation_failed, Entered an object/array twice.}
   */
  auto Enter() -> ParserResult {
    ParserResult result{ParserState::kRunning};

    if (!this->IsEntered()) {
      entered_ = true;
    } else {
      result.EmplaceError(JsonErrc::user_validation_failed, "Did not expect nested elements");
    }

    return result;
  }

  /*!
   * \brief Try to leave an object/array
   * \return Finished in case an object/array had been entered
   * \error{JsonErrorDomain, JsonErrc::user_validation_failed, Cannot leave level.}
   */
  auto Leave() -> ParserResult {
    ParserResult result{ParserState::kFinished};

    if (this->IsEntered()) {
      entered_ = false;
    } else {
      result.EmplaceError(JsonErrc::user_validation_failed, "Cannot leave level");
    }

    return result;
  }

  /*!
   * \brief True if level has been entered
   * \return True if level has been entered
   */
  auto IsEntered() const -> bool { return this->entered_; }

 private:
  /*!
   * \brief Signals that a level has been entered
   */
  bool entered_{false};
};
}  // namespace internal
}  // namespace reader
}  // namespace vajson
#endif  // LIB_VAJSON_INCLUDE_VAJSON_READER_INTERNAL_LEVEL_VALIDATOR_H_
