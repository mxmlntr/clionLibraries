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
/*!        \file  state_pool.h
 *        \brief  The header file for the statemachine to get the current state and verify the validity.
 *
 *      \details  GetState returns state of given handle and IsValid checks if handle refers to an existing state.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_POOL_H_
#define LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_POOL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

namespace vac {
namespace statemachine {

/*!
 * \brief State Pool. The state pool is responsible for resources of states.
 * \trace CREQ-158649
 */
template <class State>
class StatePool {
 public:
  /*!
   * \brief Type of the State Handle.
   */
  using Handle = typename State::HandleType;

  /*!
   * \brief Default constructor.
   */
  StatePool() = default;

  /*!
   * \brief Default copy constructor.
   */
  StatePool(const StatePool&) = default;

  /*!
   * \brief Default move constructor.
   */
  StatePool(StatePool&&) = default;

  /*!
   * \brief Default copy assignment.
   */
  StatePool& operator=(const StatePool&) = default;

  /*!
   * \brief Default move assignment.
   */
  StatePool& operator=(StatePool&&) = default;

  /*!
   * \brief Default destructor.
   */
  virtual ~StatePool() = default;

  /*!
   * \brief  Returns State of given handle.
   * \param  state_handle The state handle.
   * \return The State with given handle. If handle does not refer to a State,
   *         exception of type std::out_of_range is thrown.
   */
  virtual State* GetState(const Handle state_handle) = 0;

  /*!
   * \brief  Checks if handle refers to an existing state.
   * \param  state_handle The state handle.
   * \return True if state exists with given handle, otherwise returns false.
   */
  virtual bool IsValid(const Handle state_handle) const = 0;
};

}  // namespace statemachine
}  // namespace vac
#endif  // LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_POOL_H_
