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
/**       \file     writer.h
 *        \brief    A single include to provide all JSON serialization utilities
 *
 *        \details  Provides serializers for native JSON types & the serialization framework
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "vajson/writer/types/array_type.h"
#include "vajson/writer/types/basic_types.h"
#include "vajson/writer/types/object_type.h"

#include "vajson/writer/serializers/structures/generic_value_serializer.h"
#include "vajson/writer/serializers/structures/key_serializer.h"
#include "vajson/writer/serializers/structures/serializer.h"

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_H_
