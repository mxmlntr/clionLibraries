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
/*!        \file  threadsafe_timer_manager.h
 *        \brief  Header file for the ThreadSafeTimerManager class.
 *
 *      \details  This is a thread-safe version of the TimerManager class.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_TIMER_THREADSAFE_TIMER_MANAGER_H_
#define LIB_VAC_INCLUDE_VAC_TIMER_THREADSAFE_TIMER_MANAGER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <mutex>
#include <utility>
#include "vac/timer/timer_manager.h"

namespace vac {
namespace timer {

class Timer;

/*!
 * \brief A thread-safe timer manager.
 * \trace CREQ-200318
 */
class ThreadSafeTimerManager : public TimerManager {
 public:
  /*!
   * \brief Constructor for a ThreadSafeTimerManager linked to a given reactor.
   */
  explicit ThreadSafeTimerManager(TimerReactorInterface* reactor);
  ThreadSafeTimerManager(ThreadSafeTimerManager const&) = delete;
  ThreadSafeTimerManager& operator=(ThreadSafeTimerManager const&) & = delete;
  ThreadSafeTimerManager(ThreadSafeTimerManager&&) = delete;
  ThreadSafeTimerManager&& operator=(ThreadSafeTimerManager&&) & = delete;

  /*!
   * \brief Destructor.
   */
  ~ThreadSafeTimerManager() override;

  /*!
   * \brief   Add a timer to be considered when computing the next expiry.
   * \details Blocking, threadsafe operation. Must not call the expiry handler in its context.
   * \param   timer Pointer to a Timer object.
   */
  void AddTimer(Timer* const timer) override;

  /*!
   * \brief   No longer consider a timer when computing the next expiry.
   * \details Blocking, threadsafe operation. Must not call the expiry handler in its context.
   * \param   timer Pointer to a Timer object.
   */
  void RemoveTimer(Timer const* timer) override;

  /*!
   * \brief   Return a pair of bool and timeval struct for the next expiring timer.
   * \details Blocking, threadsafe operation. Must not call the expiry handler in its context.
   * \return  A pair of bool (if valid NextExpiry) and timeval (relative to the current time representing the next
   *          expiring timer if valid).
   */
  std::pair<bool, struct timeval> const GetNextExpiry() const override;

  /*!
   * \brief   Callback to trigger firing timers.
   * \details Threadsafe, as long as no other thread calls a blocking operation within the context of this method.
   *          Violating this constraint will result in a deadlock.
   */
  void HandleTimerExpiry() override;

 private:
  /*!
   * \brief Mutex to protect operations on the list of timers.
   */
  mutable std::recursive_mutex timer_list_mutex_;
};

}  // namespace timer
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_TIMER_THREADSAFE_TIMER_MANAGER_H_
