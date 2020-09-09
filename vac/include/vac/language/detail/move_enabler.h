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
/*!        \file  move_enabler.h
 *        \brief  Helper template for enabling/disabling move constructor/assignment.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_MOVE_ENABLER_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_MOVE_ENABLER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

namespace vac {
namespace language {
namespace detail {

/*!
 * \brief   Helper template for enabling/disabling move constructor/assignment.
 * \details Defaulting move constructor/assignment in a class with the MoveEnabler as base class will automatically
 *          enable/disable move constructor/assignment depending on compile time flag.
 * \remark  This helper should only be used in case of private inheritance.
 * \tparam  enable Switch for enabling/disabling move constructor/assignment.
 */
template <bool enable>
/* VECTOR Next Line AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
struct MoveEnabler {};

/*!
 * \brief Specialization disabling move constructor/assignment.
 */
template <>
/* VECTOR Next Line AutosarC++17_10-A12.4.2: MD_VAC_A12.4.2_ifAPublicDestructorIsNotVirtual */
struct MoveEnabler<false> {
  /*! \brief Default constructor. */
  constexpr MoveEnabler() noexcept = default;
  /*!
   * \brief   Default copy constructor deleted.
   * \details Is also deleted, because C++ may derive a move constructor from a copy.
   */
  MoveEnabler(MoveEnabler const&) noexcept = delete;
  /*! \brief Default move constructor deleted. */
  MoveEnabler(MoveEnabler&&) noexcept = delete;
  /*!
   * \brief   Default copy assignment operator deleted.
   * \details Is also deleted, because C++ may derive a move constructor from a copy.
   * \return
   */
  MoveEnabler& operator=(MoveEnabler const&) noexcept = delete;
  /*! \brief Default move assignment operator deleted. */
  MoveEnabler& operator=(MoveEnabler&&) noexcept = delete;
  /*! \brief Default destructor. */
  /* VECTOR Next Line AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
  ~MoveEnabler() noexcept = default;
};

}  // namespace detail
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_MOVE_ENABLER_H_
