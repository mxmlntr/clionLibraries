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
/*!        \file  timer_manager.h
 *        \brief  Header file for TimerManager.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_TIMER_TIMER_MANAGER_H_
#define LIB_VAC_INCLUDE_VAC_TIMER_TIMER_MANAGER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <sys/time.h>
#include <cstddef>
#include <queue>
#include <utility>
#include <vector>
#include "vac/testing/test_adapter.h"
#include "vac/timer/timer_reactor_interface.h"

namespace vac {
namespace timer {

class Timer;

/*!
 * \brief  Comparison operator that compares Timer pointers based on their expiry timestamps.
 * \param  a Left timer.
 * \param  b Right timer.
 * \return True, in case a expires after b. False otherwise.
 */
bool TimerExpiryCompare(const Timer* a, const Timer* b);

/*!
 * \brief   An event queue for Timer objects.
 * \details Not threadsafe. Use ThreadSafeTimerManager if multiple threads can interact with timers.
 * \trace   CREQ-158641
 */
class TimerManager {
 public:
  /*!
   * \brief Constructor for a TimerManager linked to a given reactor.
   */
  explicit TimerManager(TimerReactorInterface* reactor);
  TimerManager(TimerManager const&) = delete;
  TimerManager& operator=(TimerManager const&) & = delete;
  TimerManager(TimerManager&&) = delete;
  TimerManager&& operator=(TimerManager&&) & = delete;

  /*!
   * \brief Destructor.
   */
  virtual ~TimerManager();

  /*!
   * \brief   Add a timer to be considered when computing the next expiry.
   * \details Adding the same timer more than once will not create duplicates.
   * \param   timer Pointer to a Timer object.
   * \trace   CREQ-158641
   */
  virtual void AddTimer(Timer* const timer);

  /*!
   * \brief No longer consider a timer when computing the next expiry.
   * \param timer Pointer to a Timer object.
   */
  virtual void RemoveTimer(Timer const* timer);

  /*!
   * \brief  Return a pair of bool and timeval struct for the next expiring timer.
   * \return A pair of bool (if valid NextExpiry) and timeval (relative to the current time representing the next
   *         expiring timer if valid).
   */
  virtual std::pair<bool, struct timeval> const GetNextExpiry() const;

  /*!
   * \brief Callback to trigger firing timers.
   */
  virtual void HandleTimerExpiry();

  /*!
   * \brief Signals the TimerManager that any Timer has made changes to its expiry point.
   *        Timers are sorted by their expiry time.
   */
  void Update();

  /*!
   * \brief  Determine whether there are any times currently running on this TimerManager.
   * \return True if there are active timers, false otherwise.
   */
  bool empty() const { return timers_.empty(); }

  /*!
   * \brief  Determine whether there are any times currently running on this TimerManager.
   * \return True if there are active timers, false otherwise.
   */
  std::size_t size() const { return timers_.size(); }

 private:
  /*!
   * \brief Get the next timer that will fire.
   */
  Timer* GetNextTimer() const;

  /*!
   * \brief Container for storing active timers.
   */
  using TimerContainer = std::vector<Timer*>;

  /*!
   * \brief The reactor which is linked to the timer manager.
   */
  TimerReactorInterface* reactor_;

  /*!
   * \brief The set of timers to consider for firing.
   */
  TimerContainer timers_;

  FRIEND_TEST(TimerManager, AddTimer_MultipleDeadlines_1);
  FRIEND_TEST(TimerManager, AddTimer_MultipleDeadlines_2);
};

}  // namespace timer
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_TIMER_TIMER_MANAGER_H_
