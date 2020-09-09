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
/*!        \file  remove_shared_ptr.h
 *        \brief  Type trait for removing shared_ptr from given type.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_REMOVE_SHARED_PTR_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_REMOVE_SHARED_PTR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>

namespace vac {
namespace language {

/*!
 * \brief  Type trait for removing shared_ptr from given type, can be used like std::remove_pointer.
 * \tparam T Value type of shared pointer.
 * \trace  CREQ-168350
 */
template <class T>
struct remove_shared_pointer {
  /*!
   * \brief Internal typedef for the trait.
   */
  using type = T;
};

/*!
 * \copydoc remove_shared_pointer
 * \tparam  T Value type of shared pointer.
 * \trace   CREQ-168350
 */
template <class T>
struct remove_shared_pointer<std::shared_ptr<T>> {
  /*!
   * \brief Internal typedef for the trait.
   */
  using type = T;
};

/*!
 * \copydoc remove_shared_pointer
 * \tparam  T Value type of shared pointer.
 * \trace   CREQ-168350
 */
template <class T>
struct remove_shared_pointer<std::shared_ptr<T> const> {
  /*!
   * \brief Internal typedef for the trait.
   */
  using type = T;
};

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_REMOVE_SHARED_PTR_H_
