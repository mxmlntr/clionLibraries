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
/*!        \file  container_aggregator.h
 *        \brief  Aggregated Container provides access to multiple containers of the
 *                same type and an iterator for going through the aggregated elements of all containers.
 *
 *      \details  Aggregated container provides iterators for container, element and basic operators.
 *                The Span container is the only container being used now.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_CONTAINER_AGGREGATOR_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_CONTAINER_AGGREGATOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "ara/core/span.h"

namespace vac {
namespace container {

/*!
 * \brief Meta container class which allows for iterating over all elements of multiple containers of the same type.
 * \trace CREQ-161251
 */
template <typename ContainerPointer>
class ContainerAggregator final {
 public:
  /*!
   * \brief Pointer type to containers (can be raw pointer, unique_ptr, shared_ptr).
   */
  using ContainerPointerType = ContainerPointer;
  /*!
   * \brief Type of containers to be aggregated.
   */
  using ContainerType = typename std::pointer_traits<ContainerPointerType>::element_type;
  /*!
   * \brief Value type of container elements.
   */
  using value_type = typename ContainerType::value_type;

  /*!
   * \brief Const iterator type.
   */
  class const_iterator final {
   public:
    /* VECTOR Next Construct AutosarC++17_10-M0.1.5: MD_VAC_M0.1.5_UnusedTypeDeclaration */
    /*! \brief Category. */
    using iterator_category = std::forward_iterator_tag;
    /*! \brief Value type. */
    using value_type = ContainerAggregator::value_type;
    /* VECTOR Next Construct AutosarC++17_10-M0.1.5: MD_VAC_M0.1.5_UnusedTypeDeclaration */
    /*! \brief Difference type. */
    using difference_type = std::ptrdiff_t;
    /* VECTOR Next Construct AutosarC++17_10-M0.1.5: MD_VAC_M0.1.5_UnusedTypeDeclaration */
    /*! \brief Pointer type. */
    using pointer = value_type const*;
    /*! \brief Reference type. */
    using reference = value_type const&;

    /*!
     * \brief Range type for containers.
     */
    using container_range = ara::core::Span<const ContainerPointer>;
    /*!
     * \brief  Iterator for containers.
     * \remark This is a const_iterator of Span, and is therefore a raw pointer (to
     *         shared/unique/raw pointer).
     */
    using container_iterator = typename container_range::const_iterator;
    /*!
     * \brief  Iterator for container elements.
     * \remark This is an iterator of the container type, which is an std::iterator and
     *         therefore default initialized.
     */
    using element_iterator = typename ContainerType::const_iterator;

    /* VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor */
    /*!
     * \brief Default constructor.
     */
    const_iterator() : container_end_{nullptr}, container_iter_{nullptr}, element_iter_{} {}

    /*!
     * \brief Constructor.
     * \param container_range Containers to be iterated.
     * \param container_iter The containers iterator.
     * \param element_iter The elements iterator.
     */
    const_iterator(container_range const& container_range, container_iterator const& container_iter,
                   element_iterator const& element_iter)
        : container_end_{container_range.cend()}, container_iter_{container_iter}, element_iter_{element_iter} {}

    /*!
     * \brief       Compare two iterators for equality.
     * \param       other Iterator to compare to.
     * \return      True if both iterators point to the same map node.
     * \internal
     *              - #10 For equality, the container iterators must point to the same container, the element iterator
     *                    must point to the same element.
     * \endinternal
     */
    bool operator==(const_iterator const& other) const {
      return (container_iter_ == other.container_iter_) && (element_iter_ == other.element_iter_);
    }

    /*!
     * \brief  Compare two iterators for inequality.
     * \param  other Iterator to compare to.
     * \return True if both iterators point to different map nodes.
     */
    bool operator!=(const_iterator const& other) const { return !(*this == other); }

    /*!
     * \brief       Increment the iterator by one element.
     * \return      Incremented iterator.
     * \internal
     *              - #10 Check if the container iterator if there are containers after the current container iterator.
     *              - #20 Check if the element iterator is pointing to the end of the current container after
     *                    incrementing.
     *                - #21 Go to the next container containing at least one element.
     *                  - #22 Check if there are containers after the current container iterator.
     *                    - #23 Return default iterator if end of all elements is reached.
     *                - #25 Set the element iterator to the begin of next container containing at least one element.
     *              - #30 If the element iterator is NOT pointing to the end of the current container, nothing else
     *                    needs to be done.
     * \endinternal
     */
    const_iterator& operator++() {
      // #10 Check if the container iterator if there are containers after the current container iterator.
      if (container_iter_ != container_end_) {
        ++element_iter_;
        // #20 Check if the element iterator is pointing to the end of the current container after incrementing.
        if (element_iter_ == (*container_iter_)->cend()) {
          bool done{false};
          // #21 Go to the next container containing at least one element.
          do {
            ++container_iter_;
            // #22 Check if there are containers after the current container iterator.
            if (container_iter_ == container_end_) {
              // #23 Return default iterator if end of all elements is reached.
              *this = const_iterator{};
              done = true;
              break;
            }
          } while ((*container_iter_)->size() == 0);
          // #25 Set the element iterator to the begin of next container containing at least one element.
          if (!done) {
            element_iter_ = (*container_iter_)->cbegin();
          }
        }
        // #30 If the element iterator is NOT pointing to the end of the current container,
        // nothing else needs to be done.
      }
      return *this;
    }

    /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_posfixIncrementUsesInt */
    /*!
     * \brief  Post increment the iterator by one element.
     * \return Iterator prior to increment.
     */
    const_iterator operator++(int) {
      const_iterator ret{*this};
      this->operator++();
      return ret;
    }

    /*!
     * \brief Dereference operator.
     */
    reference operator*() const { return *element_iter_; }

   private:
    /*! \brief End of the view to container pointers. */
    container_iterator container_end_;
    /*! \brief Current container pointer iterator. */
    container_iterator container_iter_;
    /*! \brief Current iterator of an container element. */
    element_iterator element_iter_;
  };

  /* VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor */
  /*!
   * \brief Constructor.
   * \param container_pointer_buffer View to buffer of container pointers.
   */
  explicit ContainerAggregator(ara::core::Span<ContainerPointerType const> container_pointer_buffer)
      : registered_containers_{container_pointer_buffer} {}

  /*!
   * \brief Constructor.
   * \param container_pointer_buffer View to buffer of container pointers.
   */
  explicit ContainerAggregator(ara::core::Span<ContainerPointerType> container_pointer_buffer)
      : registered_containers_{container_pointer_buffer} {}

  /*!
   * \brief  Returns the current number of containers registered in the aggregator.
   * \return Number of containers.
   */
  std::size_t NumberOfRegisteredContainers() const { return registered_containers_.size(); }

  /*!
   * \brief  Returns the total number of container elements.
   * \return Number of container elements.
   */
  std::size_t size() const {
    return std::accumulate(
        registered_containers_.cbegin(), registered_containers_.cend(), static_cast<std::size_t>(0),
        [](std::size_t si, ContainerPointerType const& container) { return si + container->size(); });
  }

  /*!
   * \brief       Returns the first container element.
   * \return      The first element of a non-empty container, if no containers were added to the aggregator or all
   *              containers are empty cend() is returned.
   * \internal
   *              - #10 Iterate over the container list until the first container with at least one element is reached.
   *              - #20 Return cend() if all containers are empty.
   * \endinternal
   */
  const_iterator cbegin() const {
    const_iterator ret_value{cend()};
    // #10 Iterate over the container list until the first container with at least one element.
    for (typename ara::core::Span<ContainerPointer const>::const_iterator iter{registered_containers_.cbegin()};
         iter != registered_containers_.cend(); ++iter) {
      if ((**iter).size() > 0) {
        ret_value = const_iterator(registered_containers_, iter, (**iter).cbegin());
        break;
      }
    }
    // #20 Return cend() if all containers are empty.
    return ret_value;
  }

  /*!
   * \brief  Returns an iterator pointing to element after the last element after the last container.
   * \return An iterator pointing to the sentinel end value.
   */
  const_iterator cend() const { return const_iterator{}; }

  /*!
   * \brief  Returns the first container element.
   * \return The first element of a non-empty container, if no containers were added to the aggregator or all
   *         containers are empty cend() is returned.
   */
  const_iterator begin() const { return cbegin(); }

  /*!
   * \brief  Returns an iterator pointing to element after the last element after the last container.
   * \return An iterator pointing to the sentinel end value.
   */
  const_iterator end() const { return cend(); }

 private:
  /*! \brief View to the registered container pointers. */
  ara::core::Span<ContainerPointer const> registered_containers_;
};

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_CONTAINER_AGGREGATOR_H_
