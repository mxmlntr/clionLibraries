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
/**        \file  serializer.h
 *        \brief  Contains common types and forward declarations
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STRUCTURES_SERIALIZER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STRUCTURES_SERIALIZER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <ostream>

#include "vajson/util/types.h"

namespace vajson {
namespace writer {
inline namespace serializers {
/*!
 * \brief State that indicates if the object to be serialized is empty or not
 *
 * \vprivate
 */
enum class SerializerState : bool { kEmpty, kNonEmpty };

/*!
 * \brief Empty Type that signifies that the serializer has no follow up state
 *
 * \vprivate
 */
class Unit {
 public:
  /*!
   * \brief Construct this empty value with a ostream & state to satisfy the 'Next' state interface
   */
  explicit Unit(std::reference_wrapper<std::ostream>, SerializerState = SerializerState::kEmpty) noexcept {}
};

/*!
 * \brief A marker struct that only tells the GenericValueSerializer to return itself after using operator<<
 *
 * \vprivate
 */
class Self {};

/*!
 * \brief Forward declaration for the GenericValueSerializer
 * \tparam Return The type returned after using operator<<
 */
template <typename Return = Self>
class GenericValueSerializer;

/*!
 * \brief Serializes single values
 *
 * \vpublic
 */
using ValueSerializer = GenericValueSerializer<Unit>;

/*!
 * \brief Make obvious that the beginning of a JSON document is a single value
 *
 * \vpublic
 */
using DocumentSerializer = ValueSerializer;

/***********************
 * Object Declarations *
 **********************/

class KeySerializer;

/*!
 * \brief Typedef for the initial Object serializer state
 *
 * \vpublic
 */
using ObjectStart = KeySerializer;

/*!
 * \brief This class allows only for adding a value into the object
 * \details the next concatenation will allow only a key
 *
 * \vpublic
 */
using ObjectSerializerValue = GenericValueSerializer<KeySerializer>;

/**********************
 * Tuple Declarations *
 *********************/

/*!
 * \brief Serializes multiple, potentially inhomogeneous values
 *
 * \vpublic
 */
using ArraySerializer = GenericValueSerializer<>;

/*!
 * \brief Typedef for the initial Array serializer state
 *
 * \vpublic
 */
using ArrayStart = ArraySerializer;
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_STRUCTURES_SERIALIZER_H_
