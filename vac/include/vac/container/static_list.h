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
/*!        \file  static_list.h
 *        \brief  Implementation of static list.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_LIST_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_LIST_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include "vac/container/intrusive_list.h"
#include "vac/language/cpp14_backport.h"
#include "vac/memory/object_pool.h"

namespace vac {
namespace container {

/*!
 * \brief Class to implement a StaticList.
 *        List Nodes must inherit from StaticListNode<T>.
 * \trace CREQ-158594, CREQ-158596
 */
template <typename T, typename alloc = vac::memory::PhaseManagedAllocator<T>>
class StaticList final {
 public:
  /*!
   * \brief Type for nodes of the static list.
   *        Derive from this type to create an object that can be a list member.
   *        T should be the type of your object.
   */
  class Node final : public vac::container::IntrusiveListNode<Node> {
   public:
    /*!
     * \brief Constructor for a node with arguments.
     * \param args Elements which to put into the list.
     */
    template <typename... Args>
    explicit Node(Args&&... args) : vac::container::IntrusiveListNode<Node>(), elem_(std::forward<Args>(args)...) {}

    /*!
     * \brief Constructor for a node without argument.
     */
    Node() : vac::container::IntrusiveListNode<Node>(), elem_() {}

    /*!
     * \brief Default copy constructor deleted.
     */
    Node(Node const&) = delete;
    /*!
     * \brief Default move constructor deleted.
     */
    Node(Node&&) = delete;
    /*!
     * \brief Default copy assignment operator deleted.
     */
    Node& operator=(Node const&) & = delete;
    /*!
     * \brief Default move assignment deleted.
     */
    Node& operator=(Node&&) & = delete;

    /*!
     * \brief Default destructor.
     */
    ~Node() final = default;

    /*!
     * \brief  Getter for stored element.
     * \return A reference to stored element.
     */
    T& GetElem() { return elem_; }

    /*!
     * \brief  Getter for stored element.
     * \return A reference to stored element.
     */
    T const& GetElem() const { return elem_; }

   private:
    /*!
     * \brief Object with the business logic type.
     */
    T elem_;
  };

  /*!
   * \brief The memory management type used to allocate Node objects.
   *        We use the movable ObjectPoolImpl here to enable moveability of this frontend object.
   */
  using StorageType = vac::memory::MoveableObjectPool<Node, alloc>;

  /*!
   * \brief Typedef for the size type.
   */
  using size_type = typename StorageType::size_type;

  /*!
   * \brief Type of contained element.
   */
  using value_type = T;

  /*!
   * \brief Typedef for the element reference type.
   */
  using reference = T&;

  /*!
   * \brief Typedef for the element reference type.
   */
  using const_reference = T const&;

  /*!
   * \brief Typedef for the element pointer type.
   */
  using pointer = T*;

  /*!
   * \brief Typedef for the element pointer type.
   */
  using const_pointer = const T*;

  /*!
   * \brief The type of the list used to store elements.
   */
  using ListType = vac::container::IntrusiveList<Node>;

  /*!
   * \brief Typedef for the iterator type of this list.
   * \trace CREQ-160857
   */
  class iterator final {
   public:
    /*! \brief Category. */
    using iterator_category = std::bidirectional_iterator_tag;
    /*! \brief Value type. */
    using value_type = T;
    /*! \brief Difference type. */
    using difference_type = std::ptrdiff_t;
    /*! \brief Pointer type. */
    using pointer = T*;
    /*! \brief Reference type. */
    using reference = T&;

    /*! \brief Const reference type. */
    using const_reference = T const&;

    /*!
     * \brief Typedef for the iterator of the container.
     */
    using base_iterator = typename ListType::iterator;

    /*!
     * \brief Construct an iterator from a base_iterator.
     * \param it Base iterator from which to create this iterator.
     */
    explicit iterator(base_iterator const& it) : it_(it) {}

    /*!
     * \brief Construct an iterator from a base_iterator.
     * \param it Base iterator from which to create this iterator.
     */
    explicit iterator(base_iterator&& it) : it_(std::move(it)) {}

    /*!
     * \brief  Advance the iterator by one element.
     * \return A copy of the moved iterator.
     */
    iterator& operator++() {
      ++it_;
      return *this;
    }

    /*!
     * \brief  Move the iterator back by one element.
     * \return A copy of the moved iterator.
     */
    iterator& operator--() {
      --it_;
      return *this;
    }

    /*!
     * \brief  Get the list node.
     * \return A reference to the list node pointed to by this iterator.
     */
    reference operator*() { return it_->GetElem(); }

    /*!
     * \brief  Get the list node.
     * \return A reference to the list node pointed to by this iterator.
     */
    const_reference operator*() const { return it_->GetElem(); }

    /*!
     * \brief  Get the list node.
     * \return A reference to the list node pointed to by this iterator.
     */
    pointer operator->() { return &it_->GetElem(); }

    /*!
     * \brief  Compare two iterators for equality.
     * \param  other Iterator to compare to.
     * \return True if both iterators point to the same list node.
     */
    bool operator==(iterator const& other) const { return it_ == other.it_; }

    /*!
     * \brief  Compare two iterators for inequality.
     * \param  other Iterator to compare to.
     * \return True if both iterators point to different list nodes.
     */
    bool operator!=(iterator const& other) const { return it_ != other.it_; }

    /*!
     * \brief  Access to the underlying containers' iterator.
     * \return A copy of the underlying iterator.
     */
    base_iterator GetBaseIterator() const { return it_; }

   private:
    /*!
     * \brief The underlying containers' iterator.
     */
    base_iterator it_;
  };

  /*!
   * \brief Typedef for the const iterator type of this list.
   */
  class const_iterator final {
   public:
    /*! \brief Category. */
    using iterator_category = std::bidirectional_iterator_tag;
    /*! \brief Value type. */
    using value_type = T;
    /*! \brief Difference type. */
    using difference_type = std::ptrdiff_t;
    /*! \brief Pointer type. */
    using pointer = T const*;
    /*! \brief Reference type. */
    using reference = T const&;

    /*!
     * \brief Typedef for the iterator of the container.
     */
    using base_iterator = typename ListType::const_iterator;

    /*!
     * \brief Construct an iterator from a base_iterator.
     * \param it The base_iterator from which to construct the iterator.
     */
    explicit const_iterator(base_iterator const& it) : it_(it) {}

    /*!
     * \brief Construct an iterator from a base_iterator.
     * \param it The base_iterator from which to construct the iterator.
     */
    explicit const_iterator(base_iterator&& it) : it_(std::move(it)) {}

    /*!
     * \brief  Advance the iterator by one element.
     * \return A reference to the iterator.
     */
    const_iterator& operator++() {
      ++it_;
      return *this;
    }

    /*!
     * \brief  Move the iterator back by one element.
     * \return A reference to the iterator.
     */
    const_iterator& operator--() {
      --it_;
      return *this;
    }

    /*!
     * \brief  Get the list node.
     * \return A reference to the list node pointed to by this iterator.
     */
    reference operator*() const { return it_->GetElem(); }

    /*!
     * \brief  Get the list node.
     * \return A reference to the list node pointed to by this iterator.
     */
    pointer operator->() const { return &it_->GetElem(); }

    /*!
     * \brief  Compare two iterators for equality.
     * \param  other Iterator to compare to.
     * \return True if both iterators point to the same list node.
     */
    bool operator==(const_iterator const& other) const { return it_ == other.it_; }

    /*!
     * \brief  Compare two iterators for inequality.
     * \param  other Iterator to compare to.
     * \return True if both iterators point to different list nodes.
     */
    bool operator!=(const_iterator const& other) const { return it_ != other.it_; }

    /*!
     * \brief  Access to the underlying containers' iterator.
     * \return The underlying iterator.
     */
    base_iterator GetBaseIterator() const { return it_; }

   private:
    /*!
     * \brief The underlying containers' iterator.
     */
    base_iterator it_;
  };

  /*!
   * \brief Constructor to create an empty StaticList.
   */
  StaticList() = default;

  /*!
   * \brief Default copy constructor deleted.
   */
  StaticList(StaticList const&) = delete;

  /*!
   * \brief  Default copy assignment operator deleted.
   */
  StaticList& operator=(StaticList const&) & = delete;

  /*!
   * \brief Move constructor.
   */
  StaticList(StaticList&& other) : storage_(std::move(other.storage_)), queue_() {
    // splice others' list into this
    queue_.splice(queue_.end(), other.queue_);
  }

  /*!
   * \brief   Destructor that clears all elements.
   */
  ~StaticList() { clear(); }

  /*!
   * \brief  Move assignment.
   * \param  other The StaticList to move from.
   * \return A reference to the assigned-to object.
   */
  StaticList& operator=(StaticList&& other) & {
    swap(storage_, other.storage_);

    swap(queue_, other.queue_);

    return *this;
  }

  /*!
   * \brief Update the memory allocation.
   *        The current implementation only allows a single allocation. All further reservations will only limit the
   *        visible memory. All subsequent calls to reserve() where new_capacity is greater than the initial
   *        new_capacity will be rejected with a bad_alloc. \param new_capacity The number of T's to reserve space for.
   * \throw std::runtime_error Exception is thrown if the list is being reallocated.
   * \trace CREQ-158592
   */
  void reserve(size_type new_capacity) { storage_.reserve(new_capacity); }

  /*!
   * \brief Insert an element at the front of the list.
   * \param args Arguments to instantiate new Object.
   * \throw std::runtime_error  The list is full and no Object can be pushed.
   */
  template <typename... Args>
  void push_front(Args&&... args) {
    typename StorageType::pointer element_ptr{storage_.create(std::forward<Args>(args)...)};
    queue_.push_front(*element_ptr);
  }

  /*!
   * \brief Remove an element from the front of the list.
   */
  void pop_front() {
    if (!empty()) {
      Node& node_front{*queue_.front().GetSelf()};
      static_cast<void>(queue_.pop_front());
      storage_.destroy(&node_front);
    }
  }

  /*!
   * \brief  Returns a reference to the first element of the static list.
   *         Calling this function on an empty container causes undefined behavior.
   * \return Reference to a Node.
   */
  T& front() {
    // undefined behavior on empty list.
    return queue_.front().GetSelf()->GetElem();
  }

  /*!
   * \brief  Returns a reference to the first element of the static list.
   *         Calling this function on an empty container causes undefined behavior
   * \return Reference to a Node.
   */
  T const& front() const {
    // undefined behavior on empty list.
    return queue_.front().GetSelf()->GetElem();
  }

  /*!
   * \brief Insert an element at the back of the list.
   * \param value Object to copy into the list.
   * \throw std::runtime_error The list is full and no Object can be pushed.
   */
  void push_back(T const& value) {
    typename StorageType::pointer element_ptr{storage_.create(value)};
    queue_.push_back(*element_ptr);
  }

  /*!
   * \brief Insert an element at the back of the list.
   * \param value Object to move into the list.
   * \throw std::runtime_error The list is full and no Object can be pushed.
   */
  void push_back(T&& value) {
    typename StorageType::pointer element_ptr{storage_.create(std::move(value))};
    queue_.push_back(*element_ptr);
  }

  /*!
   * \brief Appends a new element to the end of the container.
   * \param args Arguments to instantiate new Object.
   * \throw std::runtime_error The list is full and no Object can be pushed.
   */
  template <typename... Args>
  void emplace_back(Args&&... args) {
    typename StorageType::pointer element_ptr{storage_.create(std::forward<Args>(args)...)};
    queue_.push_back(*element_ptr);
  }
  /*!
   * \brief Remove an element from the back of the list.
   */
  void pop_back() {
    if (!empty()) {
      Node& node_back{*queue_.back().GetSelf()};
      static_cast<void>(queue_.pop_back());
      storage_.destroy(&node_back);
    }
  }
  /*!
   * \brief  Returns a reference to the last element of the static list.
   *         Calling this function on an empty container causes undefined behavior.
   * \return Reference to a Node.
   */
  T& back() {
    // undefined behavior on empty list.
    return queue_.back().GetSelf()->GetElem();
  }

  /*!
   * \brief  Returns a reference to the last element of the static list.
   *         Calling this function on an empty container causes undefined behavior.
   * \return Reference to a Node.
   */
  T const& back() const {
    // undefined behavior on empty list.
    return queue_.back().GetSelf()->GetElem();
  }

  /*!
   * \brief  Determine whether the list is currently empty.
   * \return True if the static list is empty. False if the list has at least one element.
   */
  bool empty() const { return queue_.empty(); }

  /*!
   * \brief  Determine whether the list is currently full.
   * \return True if the static list is full. False if the list has at least one free place.
   */
  bool full() { return storage_.full(); }

  /*!
   * \brief  Iterator to the start of the list.
   * \return The iterator at the start of the list.
   */
  iterator begin() { return iterator(queue_.begin()); }

  /*!
   * \brief  Past-The-End iterator of the list.
   * \return The iterator past-the-end.
   */
  iterator end() { return iterator(queue_.end()); }

  /*!
   * \brief  Const Iterator to the start of the list.
   * \return The constant iterator at the start of the list.
   */
  const_iterator begin() const { return const_iterator(queue_.begin()); }

  /*!
   * \brief  Const Iterator to the start of the list.
   * \return The constant iterator at the start of the list.
   */
  const_iterator cbegin() const { return begin(); }

  /*!
   * \brief  Const Past-The-End iterator of the list.
   * \return The constant iterator past-the-end.
   */
  const_iterator end() const { return const_iterator(queue_.end()); }

  /*!
   * \brief  Const Past-The-End iterator of the list.
   * \return The constant iterator past-the-end.
   */
  const_iterator cend() const { return end(); }

  /*!
   * \brief  Remove an element pointed to by the iterator.
   * \param  elem Iterator pointing to the element to be removed from the list.
   * \return An iterator to the next element in the list.
   * \trace  CREQ-158593
   */
  iterator erase(iterator elem) {
    Node* elem_to_destroy{elem.GetBaseIterator().GetListNode()->GetSelf()};
    iterator next_iterator{iterator(queue_.erase(elem.GetBaseIterator()))};
    storage_.destroy(elem_to_destroy);
    return next_iterator;
  }

  /*!
   * \brief  Remove all elements that fulfill the predicate.
   * \note   Implementation according to proposed C++ 20 standard.
   * \tparam Fn The type of predicate function.
   * \param  predicate The predicate that has to be fulfilled to remove elements from the list.
   * \return The number of elements that were removed from the list.
   * \trace  CREQ-158593
   */
  template <class Fn>
  auto remove_if(Fn predicate) -> size_type {
    size_type count{0};
    iterator it{this->begin()};
    while (it != this->end()) {
      if (predicate(*it)) {
        it = this->erase(it);
        count += 1;
      } else {
        ++it;
      }
    }
    return count;
  }

  /*!
   * \brief  Remove all elements that are equal to the passed value.
   * \note   Implementation according to proposed C++ 20 standard.
   * \param  value Value that should be removed from the list.
   * \return The number of elements that were removed from the list.
   * \trace  CREQ-158593
   */
  size_type remove(T const& value) {
    return remove_if([&value](T const& candidate) { return value == candidate; });
  }

  /*!
   * \brief Removes all elements from the container.
   */
  void clear() {
    iterator it{this->begin()};
    while (it != this->end()) {
      it = this->erase(it);
    }
  }

  /*!
   * \brief  Returns the number of elements in the container.
   * \return The number of elements in the container.
   */
  size_type size() const {
    return static_cast<typename std::make_unsigned<typename std::iterator_traits<iterator>::difference_type>::type>(
        std::distance(this->begin(), this->end()));
  }

  /*!
   * \brief  Returns the total number of elements that can be allocated in this list.
   * \return The total number of elements that can be allocated in this list.
   */
  size_type capacity() const noexcept { return storage_.capacity(); }

  /*!
   * \brief Insert a new element into the list past the element pointed to by the Iterator.
   * \param where The new object is inserted past the iterator where.
   * \param args Arguments to instantiate new Object.
   * \throw std::runtime_error  The list is full and no Object can be inserted.
   * \trace CREQ-158593
   */
  template <typename... Args>
  void insert(iterator where, Args&&... args) {
    typename StorageType::pointer element_ptr{storage_.create(std::forward<Args>(args)...)};
    static_cast<void>(queue_.insert(where.GetBaseIterator(), *element_ptr));
  }

 private:
  /*!
   * \brief Array of optional Nodes used to store the elements.
   */
  StorageType storage_;

  /*!
   * \brief List of the elements.
   */
  ListType queue_;
};

/*!
 * \brief Type for nodes of the static list.
 *        Derive from this type to create an object that can be a list member.
 *        T should be the type of your object.
 */
template <typename T>
using StaticListNode = typename StaticList<T>::Node;
}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_LIST_H_
