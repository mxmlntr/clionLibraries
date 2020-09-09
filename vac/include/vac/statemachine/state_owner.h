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
/*!        \file  state_owner.h
 *        \brief  The top header file of statemachine which includes state.h and state_pool.h.
 *
 *      \details  GetState() returns the current state.
 *                TryChangeState changes current state to state associated to given handle.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_OWNER_H_
#define LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_OWNER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "vac/statemachine/state.h"
#include "vac/statemachine/state_pool.h"

namespace vac {
namespace statemachine {

/*!
 * \brief Interface for classes owning a State.
 * \trace CREQ-158649
 */
template <class State>
class StateOwner {
 public:
  /*!
   * \brief Type of the State Handle.
   */
  using Handle = typename State::HandleType;

  /*!
   * \brief Type of the State Context.
   */
  using Context = typename State::ContextType;

  /*!
   * \brief Constructor.
   */
  StateOwner(StatePool<State>& state_pool, Handle state_handle)
      : state_pool_{state_pool}, state_{state_pool_.GetState(state_handle)} {}

  /*!
   * \brief Copy constructor.
   */
  StateOwner(const StateOwner&) = delete;

  /*!
   * \brief Copy assignment.
   */
  StateOwner& operator=(const StateOwner&) = delete;

  /*!
   * \brief Move constructor.
   */
  StateOwner(StateOwner&&) = delete;

  /*!
   * \brief Move assignment.
   */
  StateOwner& operator=(StateOwner&&) = delete;

  /*!
   * \brief Destructor.
   */
  virtual ~StateOwner() = default;

  /*!
   * \brief Returns the current state.
   */
  State* GetState() { return state_; }

  /*!
   * \brief Returns the current state.
   */
  State const* GetState() const { return state_; }

  /*!
   * \brief Returns the Context used by context depending State methods.
   */
  virtual Context& GetContext() = 0;

  /*!
   * \brief  Tries to change current state to state associated to given handle.
   * \param  state_handle The handle of the state to which change is requested.
   * \return True if state change was successful, otherwise returns false.
   * \trace  CREQ-158650
   */
  bool TryChangeState(const Handle state_handle) {
    bool ret_value{false};
    if (state_->IsValidChange(state_handle)) {
      state_->OnLeave(GetContext());
      state_ = state_pool_.GetState(state_handle);
      state_->OnEnter(GetContext());
      ret_value = true;
    }
    return ret_value;
  }

 private:
  /*!
   * \brief Reference to associated State Pool.
   */
  StatePool<State>& state_pool_;

  /*!
   * \brief Pointer to current state.
   */
  State* state_;
};

}  // namespace statemachine
}  // namespace vac
#endif  // LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_OWNER_H_
