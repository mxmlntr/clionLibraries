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
/*!        \file  static_map.h
 *        \brief  Contains StaticMap class.
 *
 *      \details  The StaticMap class can be used to store items derived from StaticMapNode.
 *                Before adding nodes the number of supported nodes has to be specified.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_MAP_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_MAP_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include "vac/container/intrusive_map.h"
#include "vac/language/cpp14_backport.h"
#include "vac/memory/object_pool.h"
#include "vac/memory/phase_managed_allocator.h"

namespace vac {
namespace container {

/*!
 * \brief   Class to implement a StaticMap.
 *          Map Nodes must inherit from StaticMapNode.
 *          Before adding nodes the number of supported nodes has to be reserved.
 * \tparam  Key The key type.
 * \tparam  T The value type.
 * \remarks The execution sequence of key and the value type is not specified by C++11. Therefore do not expect a
 *          specific execution order of the key or the value constructor. Also do not use key and value objects that
 *          have dependencies to each other.
 * \trace   CREQ-158597
 */
template <typename Key, typename T>
class StaticMap final {
 public:
  /*!
   * \brief The type implementing the pair.
   */
  using value_type = std::pair<const Key, T>;

  /*!
   * \brief The key type of this map.
   */
  using key_type = Key;

  /*!
   * \brief The mapped type of this map.
   */
  using mapped_type = T;

  /*!
   * \brief Type for nodes of the static Map.
   *        Key type for key in the pair
   *        T type of the value in the pair.
   */
  class Node final : public vac::container::IntrusiveMapNode<Key, Node> {
   public:
    /*!
     * \brief Type of compare result.
     */
    using CompareType = int;

    /*!
     * \brief Constructor for a node without argument.
     */
    Node() = delete;

    /*!
     * \brief Copy constructor.
     */
    Node(Node const&) = delete;

    /*!
     * \brief Move constructor.
     * \param other_node Node to be moved.
     */
    Node(Node&& other_node) = delete;

    /*!
     * \brief  Deleted copy assignment.
     * \return A reference to the assigned to object.
     */
    Node& operator=(Node const&) & = delete;

    /*!
     * \brief  Deleted move assignment.
     * \return A reference to the assigned to object.
     */
    Node& operator=(Node&&) & = delete;

    /*!
     * \brief   Forwarding constructor.
     * \param   args Arguments for the constructor of the Key type.
     * \remarks The provided arguments might lead to an exception when the used storage's (value_type) constructor
     *          could throw exceptions. When that is the case the caller should not expect a specific execution sequence
     *          of the constructor, because the execution sequence is not specified by C++11.
     *          Therefore the constructor of the value type should not be dependent to the state of the state of the
     *          used key object and vice versa. Also do not expect a specific execution order for key and value.
     */
    template <typename... Args>
    Node(Args&&... args)  // NOLINT[runtime/explicit]
        : vac::container::IntrusiveMapNode<Key, Node>(),
          /* VECTOR Next Line AutosarC++17_10-A5.0.1: MD_VAC_A5.0.1_functionOrderStdForwardStaticMap */
          storage_(std::forward<Args>(args)...) {}

    /*!
     * \brief Destructor.
     */
    ~Node() final = default;

    /*!
     * \brief  Compares the new key value with the node current node value.
     * \param  new_key Value to be compared.
     * \return A negative value if new key is less than the other current node key, zero if the both keys are equal,
     *         positive value if new key is greater than the other current node key.
     */
    CompareType KeyCompare(StaticMap::key_type new_key) const final {
      CompareType retval{0};
      if (storage_.first < new_key) {
        retval = 1;
      } else if (new_key < storage_.first) {
        retval = -1;
      } else {
        retval = 0;
      }
      return retval;
    }

    /*!
     * \brief  Compares the new key value with the node current node value.
     * \param  key_node Node containing the key to be compared to.
     * \return A negative value if new key is less than the other current node key, zero if the both keys are equal,
     *         positive value if new key is greater than the other current node key.
     */
    CompareType KeyCompare(Node const& key_node) const final { return this->KeyCompare(key_node.storage_.first); }

    /*!
     * \brief Pair to store the key and value.
     */
    value_type storage_;
  };

  /*!
   * \brief The memory management type used to allocate Node objects.
   * \trace CREQ-158594
   */
  using StorageType = vac::memory::ObjectPool<Node, vac::memory::PhaseManagedAllocator<Node>>;

  /*!
   * \brief The size type used in this implementation.
   */
  using size_type = typename StorageType::size_type;

  /*!
   * \brief The type of the map used to store elements.
   */
  using MapType = vac::container::IntrusiveMap<Key, Node>;

  /*!
   * \brief Typedef for the iterator type of this map.
   * \trace CREQ-160857
   */
  class iterator final {
   public:
    /*! \brief Category. */
    using iterator_category = std::bidirectional_iterator_tag;
    /*! \brief Value type. */
    using value_type = StaticMap::value_type;
    /*! \brief Difference type. */
    using difference_type = std::ptrdiff_t;
    /*! \brief Pointer type. */
    using pointer = value_type*;
    /*! \brief Reference type. */
    using reference = value_type&;

    /*! \brief Const reference type. */
    using const_reference = value_type const&;
    /*! \brief Const pointer type. */
    using const_pointer = value_type const*;

    /*!
     * \brief Typedef for the iterator of the container.
     */
    using base_iterator = typename MapType::iterator;

    /*!
     * \brief Construct an iterator from a base_iterator.
     * \param it The base iterator from which to construct the object.
     */
    explicit iterator(base_iterator const& it) : it_(it) {}

    /*!
     * \brief Construct an iterator from a base_iterator.
     * \param it The base iterator from which to construct the object.
     */
    explicit iterator(base_iterator&& it) : it_(std::move(it)) {}

    /*!
     * \brief Dereference iterator to map element.
     */
    reference operator*() { return it_.GetMapNode()->GetSelf()->storage_; }

    /*!
     * \brief Dereference iterator to map element.
     */
    const_reference operator*() const { return it_.GetMapNode()->GetSelf()->storage_; }

    /*!
     * \brief Dereference iterator to map element.
     */
    pointer operator->() { return &(it_.GetMapNode()->GetSelf()->storage_); }

    /*!
     * \brief Dereference iterator to map element.
     */
    const_pointer operator->() const { return &(it_.GetMapNode()->GetSelf()->storage_); }

    /*!
     * \brief  Increment the iterator by one element.
     * \return IntrusiveMapIterator to the element with immediate higher key value.
     */
    iterator& operator++() {
      ++it_;
      return *this;
    }

    /*!
     * \brief  Decrement the iterator by one element.
     * \return IntrusiveMapIterator to the element with an immediate lower key value.
     */
    iterator& operator--() {
      --it_;
      return *this;
    }

    /*!
     * \brief  Compare two iterators for equality.
     * \param  other Iterator to compare to.
     * \return True if both iterators point to the same map node.
     */
    bool operator==(iterator const& other) const { return it_ == other.it_; }

    /*!
     * \brief  Compare two iterators for inequality.
     * \param  other Iterator to compare to.
     * \return True if both iterators point to different map nodes.
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
   * \brief Typedef for the const iterator type of this map.
   */
  class const_iterator final {
   public:
    /*! \brief Category. */
    using iterator_category = std::bidirectional_iterator_tag;
    /*! \brief Value type. */
    using value_type = StaticMap::value_type;
    /*! \brief Difference type. */
    using difference_type = std::ptrdiff_t;
    /*! \brief Pointer type. */
    using pointer = value_type const*;
    /*! \brief Reference type. */
    using reference = value_type const&;

    /*!
     * \brief Typedef for the const iterator of the container.
     */
    using base_iterator = typename MapType::const_iterator;

    /*!
     * \brief Constructor for an iterator from a base_iterator.
     * \param it The base iterator from which to construct the object.
     */
    explicit const_iterator(base_iterator const& it) : it_(it) {}

    /*!
     * \brief Constructor of an iterator from a base_iterator.
     * \param it The base iterator from which to construct the object.
     */
    explicit const_iterator(base_iterator&& it) : it_(std::move(it)) {}

    /*!
     * \brief Dereference iterator to map element.
     */
    reference operator*() const { return it_.GetMapNode()->GetSelf()->storage_; }

    /*!
     * \brief Dereference iterator to map element.
     */
    pointer operator->() const { return &(it_.GetMapNode()->GetSelf()->storage_); }

    /*!
     * \brief  Increment the iterator by one element.
     * \return IntrusiveMapIterator to the element with immediate higher key value.
     */
    const_iterator& operator++() {
      ++it_;
      return *this;
    }

    /*!
     * \brief  Decrement the iterator by one element.
     * \return IntrusiveMapIterator to the element with an immediate lower key value.
     */
    const_iterator& operator--() {
      --it_;
      return *this;
    }

    /*!
     * \brief  Compare two iterators for equality.
     * \param  other Iterator to compare to.
     * \return True if both iterators point to the same map node.
     */
    bool operator==(const_iterator const& other) const { return it_ == other.it_; }

    /*!
     * \brief  Compare two iterators for inequality.
     * \param  other Iterator to compare to.
     * \return True if both iterators point to different map nodes.
     */
    bool operator!=(const_iterator const& other) const { return it_ != other.it_; }

    /*!
     * \brief  Access to the underlying containers' iterator.
     * \return A copy to the underlying iterator.
     */
    base_iterator GetBaseIterator() const { return it_; }

   private:
    /*!
     * \brief The underlying containers' iterator.
     */
    base_iterator it_;
  };

  /*!
   * \brief Constructor to create an empty Static map.
   */
  StaticMap() = default;

  /*!
   * \brief Deleted copy constructor.
   */
  StaticMap(StaticMap const&) = delete;
  /*!
   * \brief  Deleted copy assignment.
   * \return A reference to the assigned-to object.
   */
  StaticMap operator=(StaticMap const&) & = delete;
  /*!
   * \brief Deleted move constructor.
   */
  StaticMap(StaticMap&&) = delete;
  /*!
   * \brief  Deleted move assignment.
   * \return A reference to the assigned-to object.
   */
  StaticMap operator=(StaticMap&&) & = delete;

  /*!
   * \brief Destructor.
   */
  ~StaticMap() { clear(); }

  /*!
   * \brief Update the memory allocation. The current implementation only allows a single allocation.
   *        All further reservations will only limit the visible memory. All subsequent calls to reserve()
   *        where new_capacity is greater than the initial new_capacity will be rejected with a bad_alloc.
   * \param new_capacity The number of nodes to reserve space for.
   * \throw std::runtime_error Exception is thrown if the map is being reallocated.
   * \trace CREQ-158592
   */
  void reserve(size_type new_capacity) { storage_.reserve(new_capacity); }

  /*!
   * \brief  Determine whether the map is currently empty.
   * \return True if the static map is empty. False if the map has at least one element.
   */
  bool empty() const { return map_.empty(); }

  /*!
   * \brief The number of objects currently allocated in this static map.
   */
  size_type size() const noexcept { return storage_.size(); }

  /*!
   * \brief  Determine whether the map is currently full.
   * \return True if the static map is full. False if the map has at least one free place.
   */
  bool full() const { return storage_.full(); }

  /*!
   * \brief  Return an iterator to the first element of the map.
   * \return An iterator to the first element.
   */
  iterator begin() { return iterator(map_.begin()); }

  /*!
   * \brief  Get iterator to end element.
   * \return Iterator to end element.
   */
  iterator end() { return iterator(map_.end()); }

  /*!
   * \brief  Return an iterator to the first element of the map.
   * \return An iterator to the first element.
   */
  const_iterator begin() const { return const_iterator(map_.cbegin()); }

  /*!
   * \brief  Get iterator to end element.
   * \return Iterator to end element.
   */
  const_iterator end() const { return const_iterator(map_.cend()); }

  /*!
   * \brief  Return an iterator to the first element of the map.
   * \return An iterator to the first element.
   */
  const_iterator cbegin() const { return this->begin(); }

  /*!
   * \brief  Get iterator to end element.
   * \return Iterator to end element.
   */
  const_iterator cend() const { return this->end(); }

  /*!
   * \brief  Remove an element from map.
   * \param  erase_key Key to be erased.
   * \return Number of elements removed.
   * \trace  CREQ-158593
   */
  std::size_t erase(Key const& erase_key) {
    std::size_t erased_count{0};
    iterator itr{find(erase_key)};

    if (itr != end()) {
      Node* elem_to_destroy{itr.GetBaseIterator().GetMapNode()->GetSelf()};
      erased_count = map_.erase(erase_key);
      storage_.destroy(elem_to_destroy);
    }
    return erased_count;
  }

  /*!
   * \brief Remove an element from map.
   * \param elem Node to be erased.
   */
  void erase(iterator elem) {
    /* VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect */
    if (elem != end()) {
      Node* node{elem.GetBaseIterator().GetMapNode()->GetSelf()};
      node->EraseFromMap();
      storage_.destroy(node);
    }
  }

  /*!
   * \brief Remove all elements from the map.
   */
  void clear() {
    while (!empty()) {
      erase(begin());
    }
  }

  /*!
   * \brief  Find node in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a end_ node.
   */
  iterator find(Key const& find_key) noexcept { return iterator(map_.find(find_key)); }

  /*!
   * \brief  Find node in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a end_ node.
   */
  const_iterator find(Key const& find_key) const noexcept { return const_iterator(map_.find(find_key)); }

  /*!
   * \brief  Insert a new element into the map.
   * \param  item Element to be inserted in the map.
   * \return Pair consisting of an iterator to the inserted element (or to the element that prevented the insertion)
   *         and a bool denoting whether the insertion took place.
   * \throw  std::runtime_error  The map is full and no Object can be inserted.
   * \trace  CREQ-158593
   */
  std::pair<iterator, bool> insert(value_type const& item) {
    std::pair<iterator, bool> ret_value{std::make_pair(end(), false)};
    // Check if key is already contained in map.
    iterator existing{map_.find(item.first)};
    if (existing != end()) {
      // When key already in use return pair with iterator to the element that prevented the insertion
      // and second set to false.
      ret_value = std::make_pair(existing, false);
    } else {
      // When key is not used create new element in storage and insert into map.
      typename StorageType::pointer element_ptr{storage_.create(item)};
      std::pair<typename MapType::iterator, bool> pair{map_.insert(element_ptr)};

      assert(
          pair.second &&
          ("Since the existence of the key is checked before and map_.insert returns only false if the key exists,this "
           "can never be false in a single-threaded context." != nullptr));

      // Return pair of iterator and flag (when flag is false insert failed).
      ret_value = std::make_pair(iterator(pair.first), pair.second);
    }
    return ret_value;
  }

  /*!
   * \brief   Inserts a new element into the map constructed in-place with the given args.
   *          Careful use of emplace allows the new element to be constructed in the storage used by the current
   *          instance while avoiding unnecessary copy or move operations. The constructor of the new element is called
   *          with exactly the same arguments as supplied to emplace, forwarded via std::forward<Args>(args)... to the
   *          create method of the underlying storage. The element may be constructed even if there already is an
   *          element with a matching key in the container, in this case the newly constructed element will be destroyed
   *          immediately after construction.
   * \param   args Arguments to forward to the constructor of the element.
   * \return  Pair consisting of an iterator to the inserted element (or to the element that prevented the insertion)
   *          and a bool denoting whether the insertion took place (true: inserted; false: not inserted).
   * \throw   std::bad_alloc The map is full and no Object can be inserted.
   * \remarks A std::bad_alloc exception will be thrown when the underlying storage is full. It is also possible that
   *          another exception is thrown during the emplace of the object (thrown by the constructor) to the underlying
   *          storage. When the storage is not full and the key of the created node is already used a pair will be
   *          returned (first will point to the node that prevented the insertion, second will be false).
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> std::pair<iterator, bool> {
    // Create a node instance. An exception will be thrown if the storage is full or the constructor throws.
    typename StorageType::pointer element_ptr{storage_.create(std::forward<Args>(args)...)};
    // Insert the element to the map.
    std::pair<typename MapType::iterator, bool> pair{map_.insert(element_ptr)};
    // When the key is already used the pair's second value is false.
    if (!pair.second) {
      // When insert to underlying map failed immediately destroy the create element.
      storage_.destroy(element_ptr);
    }

    // Return pair of iterator and flag (when flag is false insert failed).
    return std::make_pair(iterator(pair.first), pair.second);
  }

 private:
  /*!
   * \brief Instance of objectpool storage.
   */
  StorageType storage_;

  /*!
   * \brief Map used to store nodes.
   */
  MapType map_;
};

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_MAP_H_
