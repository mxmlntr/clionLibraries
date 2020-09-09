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
/*!        \file  serializers/vac/variant.h
 *        \brief
 *      \details
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_VAC_VARIANT_H_
#define LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_VAC_VARIANT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "ara/core/variant.h"

namespace vajson {
namespace writer {
inline namespace serializers {
/*!
 * \brief Generic Variant visitor
 * \details Will serialize the children according to their implemented serializers
 * \tparam Serializer The current Serializer type
 *
 * \vprivate
 */
template <typename Serializer>
class VariantVisitor {
  static_assert(std::is_nothrow_move_constructible<Serializer>::value, "Serializer must be noexcept movable");
  static_assert(std::is_nothrow_move_assignable<Serializer>::value, "Serializer must be noexcept movable");

 public:
  /*!
   * \brief Creates the (one time use) visitor
   * \param serializer the serializer to use for this variant
   */
  explicit VariantVisitor(Serializer&& serializer) : serializer_(std::move(serializer)) {}

  /*!
   * \brief Generic function that is invoked for any contained variant
   * \tparam T The contained variant type
   * \param value The contained value that is serialized
   * \returns the next serializer state
   */
  template <typename T>
  auto operator()(T const& value) noexcept -> typename Serializer::Next {
    return std::move(this->serializer_) << value;
  }

 private:
  /*!
   * \brief The serializer that's used to serialize the variant
   */
  Serializer serializer_;
};

/*!
 * \brief Provides a serializer for a variant of serializable elements
 * \tparam Serializer The serializer type
 * \tparam Types The Value types of the variant
 * \param s The serializer instance
 * \param variant The variant to serialize
 * \returns the next serializer state
 *
 * \vpublic
 */
template <typename Serializer, typename... Types>
auto operator<<(Serializer s, ara::core::Variant<Types...>& variant) noexcept(false) -> typename Serializer::Next {
  // VECTOR NL AutosarC++17_10-A18.9.2: MD_JSON_AutosarC++17_10-A18.9.2_serializer
  VariantVisitor<Serializer> ser{std::move(s)};
  return ara::core::visit(std::move(ser), variant);
}
}  // namespace serializers
}  // namespace writer
}  // namespace vajson

#endif  // LIB_VAJSON_INCLUDE_VAJSON_WRITER_SERIALIZERS_VAC_VARIANT_H_
