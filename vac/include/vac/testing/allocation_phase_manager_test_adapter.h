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
/*!        \file  allocation_phase_manager_test_adapter.h
 *        \brief  The header file of the phase manager.
 *
 *      \details  Include functions to set the AllocationPhase of the AllocationPhaseManager to
 *                allocation, steady and deallocation phase.
 *                THIS CLASS MUST NOT BE USED IN PRODUCTION CODE.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_TESTING_ALLOCATION_PHASE_MANAGER_TEST_ADAPTER_H_
#define LIB_VAC_INCLUDE_VAC_TESTING_ALLOCATION_PHASE_MANAGER_TEST_ADAPTER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include "vac/memory/three_phase_allocator.h"

namespace vac {
namespace testing {

/*!
 * \brief Helper function to set the AllocationPhase of the AllocationPhaseManager.
 * \param phase The new AllocationPhase.
 */
inline void SetPhase(vac::memory::AllocationPhaseManager::AllocationPhase phase) {
  vac::memory::AllocationPhaseManager::GetInstance().SetPhase(phase);
}

/*!
 * \brief Helper function to set the AllocationPhase of the AllocationPhaseManager to allocation.
 */
inline void SetAllocationPhase() { SetPhase(vac::memory::AllocationPhaseManager::AllocationPhase::allocation); }

/*!
 * \brief Helper function to set the AllocationPhase of the AllocationPhaseManager to steady.
 */
inline void SetSteadyPhase() { SetPhase(vac::memory::AllocationPhaseManager::AllocationPhase::steady); }

/*!
 * \brief Helper function to set the AllocationPhase of the AllocationPhaseManager to deallocation.
 *        This method could be used e. g. in the TearDown or destructor of a test fixture.
 */
inline void SetDeallocationPhase() { SetPhase(vac::memory::AllocationPhaseManager::AllocationPhase::deallocation); }

/*!
 * \brief This class resets the AllocationPhaseManager to a new instance.
 */
class AllocationPhaseManagerTestAdapter final {
 public:
  /*!
   * \brief Constructor that ensures that a new instance of the AllocationPhaseManager is created.
   * \trace CREQ-158645
   */
  AllocationPhaseManagerTestAdapter() {
    // Ensure that an instance exists
    vac::memory::AllocationPhaseManager& allocation_phase_manager = vac::memory::AllocationPhaseManager::GetInstance();
    // Destroy & recreate existing instance
    allocation_phase_manager.~AllocationPhaseManager();
    new (&allocation_phase_manager) vac::memory::AllocationPhaseManager();
  }

  /*!
   * \brief Copy constructor.
   */
  AllocationPhaseManagerTestAdapter(AllocationPhaseManagerTestAdapter const&) = default;

  /*!
   * \brief Copy assignment.
   */
  AllocationPhaseManagerTestAdapter& operator=(AllocationPhaseManagerTestAdapter const&) & = default;

  /*!
   * \brief Move constructor.
   */
  AllocationPhaseManagerTestAdapter(AllocationPhaseManagerTestAdapter&&) = default;

  /*!
   * \brief Move assignment.
   */
  AllocationPhaseManagerTestAdapter& operator=(AllocationPhaseManagerTestAdapter&&) & = default;

  ~AllocationPhaseManagerTestAdapter() = default;
};

}  // namespace testing
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_TESTING_ALLOCATION_PHASE_MANAGER_TEST_ADAPTER_H_
