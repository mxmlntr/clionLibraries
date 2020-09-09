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
/*!        \file  vac/language/detail/copy_move_enabler.h
 *        \brief  Helper template for enabling/disabling copy/move constructor/assignment.
 *
 *      \details  Conditionally deletes copy/move constructor/assignment based on template arguments.
 *                Used as a private member in classes which then uses default declarations
 *                for corresponding methods.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_COPY_MOVE_ENABLER_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_COPY_MOVE_ENABLER_H_

namespace vac {
namespace language {
namespace detail {

/* VECTOR Disable AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
/*!
 *  \brief Conditionally disable copy/move constructor.
 */
template <bool IsCopyConstructible, bool IsMoveConstructible>
class CopyMoveConstructorEnabler {};

/*!
 *  \brief Conditionally disable copy/move assignment.
 */
template <bool IsCopyAssignable, bool IsMoveAssignable>
class CopyMoveAssignmentEnabler {};

/* VECTOR Next Construct AutosarC++17_10-A10.1.1: MD_VAC_A10.1.1_multipleInheritance */
/* VECTOR Disable AutosarC++17_10-M10.2.1: MD_VAC_M10.2.1_uniqueEntityNamesInMultipleInheritance */
/*!
 * \brief Conditionally disables copy/move constructor/assignment.
 */
template <bool IsCopyConstructible, bool IsMoveConstructible, bool IsCopyAssignable, bool IsMoveAssignable>
class CopyMoveEnabler : private CopyMoveConstructorEnabler<IsCopyConstructible, IsMoveConstructible>,
                        private CopyMoveAssignmentEnabler<IsCopyAssignable, IsMoveAssignable> {};

/* VECTOR Disable AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
/*! \brief Disables copy constructor. */
template <>
class CopyMoveConstructorEnabler<false, true> {
 public:
  /*! \brief Default constructor. */
  constexpr CopyMoveConstructorEnabler() = default;
  /*! \brief Default copy constructor deleted. */
  constexpr CopyMoveConstructorEnabler(CopyMoveConstructorEnabler const&) = delete;
  /*! \brief Default move constructor. */
  constexpr CopyMoveConstructorEnabler(CopyMoveConstructorEnabler&&) = default;
  /*! \brief Default copy assignment. */
  CopyMoveConstructorEnabler& operator=(CopyMoveConstructorEnabler const&) & = default;
  /*! \brief Default move assignment. */
  CopyMoveConstructorEnabler& operator=(CopyMoveConstructorEnabler&&) & = default;
};

/*! \brief Disables move constructor. */
template <>
class CopyMoveConstructorEnabler<true, false> {
 public:
  /*! \brief Default constructor. */
  constexpr CopyMoveConstructorEnabler() = default;
  /*! \brief Default copy constructor. */
  constexpr CopyMoveConstructorEnabler(CopyMoveConstructorEnabler const&) = default;
  /*! \brief Default move constructor deleted. */
  constexpr CopyMoveConstructorEnabler(CopyMoveConstructorEnabler&&) = delete;
  /*! \brief Default copy assignment. */
  CopyMoveConstructorEnabler& operator=(CopyMoveConstructorEnabler const&) & = default;
  /*! \brief Default move assignment. */
  CopyMoveConstructorEnabler& operator=(CopyMoveConstructorEnabler&&) & = default;
};

/*! \brief Disables both copy and move constructor. */
template <>
class CopyMoveConstructorEnabler<false, false> {
 public:
  /*! \brief Default constructor. */
  constexpr CopyMoveConstructorEnabler() = default;
  /*! \brief Default copy constructor deleted. */
  constexpr CopyMoveConstructorEnabler(CopyMoveConstructorEnabler const&) = delete;
  /*! \brief Default move constructor deleted. */
  constexpr CopyMoveConstructorEnabler(CopyMoveConstructorEnabler&&) = delete;
  /*! \brief Default copy assignment. */
  CopyMoveConstructorEnabler& operator=(CopyMoveConstructorEnabler const&) & = default;
  /*! \brief Default move assignment. */
  CopyMoveConstructorEnabler& operator=(CopyMoveConstructorEnabler&&) & = default;
};

/*! \brief Disables copy assignment. */
template <>
class CopyMoveAssignmentEnabler<false, true> {
 public:
  /*! \brief Default constructor. */
  constexpr CopyMoveAssignmentEnabler() = default;
  /*! \brief Default copy constructor. */
  constexpr CopyMoveAssignmentEnabler(CopyMoveAssignmentEnabler const&) = default;
  /*! \brief Default move constructor. */
  constexpr CopyMoveAssignmentEnabler(CopyMoveAssignmentEnabler&&) = default;
  /*! \brief Default copy assignment operator deleted. */
  CopyMoveAssignmentEnabler& operator=(CopyMoveAssignmentEnabler const&) & = delete;
  /*! \brief Default move assignment. */
  CopyMoveAssignmentEnabler& operator=(CopyMoveAssignmentEnabler&&) & = default;
};

/*! \brief Disables move assignment. */
template <>
class CopyMoveAssignmentEnabler<true, false> {
 public:
  /*! \brief Default constructor. */
  constexpr CopyMoveAssignmentEnabler() = default;
  /*! \brief Default copy constructor. */
  constexpr CopyMoveAssignmentEnabler(CopyMoveAssignmentEnabler const&) = default;
  /*! \brief Default move constructor. */
  constexpr CopyMoveAssignmentEnabler(CopyMoveAssignmentEnabler&&) = default;
  /*! \brief Default copy assignment. */
  CopyMoveAssignmentEnabler& operator=(CopyMoveAssignmentEnabler const&) & = default;
  /*! \brief Default move assignment operator deleted. */
  CopyMoveAssignmentEnabler& operator=(CopyMoveAssignmentEnabler&&) & = delete;
};

/*! \brief Disables copy and move assignment. */
template <>
class CopyMoveAssignmentEnabler<false, false> {
 public:
  /*! \brief Default constructor. */
  constexpr CopyMoveAssignmentEnabler() = default;
  /*! \brief Default copy constructor. */
  constexpr CopyMoveAssignmentEnabler(CopyMoveAssignmentEnabler const&) = default;
  /*! \brief Default move constructor. */
  constexpr CopyMoveAssignmentEnabler(CopyMoveAssignmentEnabler&&) = default;
  /*! \brief Default copy assignment operator deleted. */
  CopyMoveAssignmentEnabler& operator=(CopyMoveAssignmentEnabler const&) & = delete;
  /*! \brief Default move assignment operator deleted. */
  CopyMoveAssignmentEnabler& operator=(CopyMoveAssignmentEnabler&&) & = delete;
};
/* VECTOR Enable AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
/* VECTOR Enable AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
/* VECTOR Enable AutosarC++17_10-M10.2.1: MD_VAC_M10.2.1_uniqueEntityNamesInMultipleInheritance */

}  // namespace detail
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_COPY_MOVE_ENABLER_H_
