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
/*!        \file  intrusive_map.h
 *        \brief  The header file of intrusive map is an implementation of a key-value-storage where the
 *                key is stored inside the value object.
 *
 *      \details  Implement the tree-like functions, for example, find parent node, find left/right nodes
 *                erase node, and insert node.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_INTRUSIVE_MAP_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_INTRUSIVE_MAP_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "vac/testing/test_adapter.h"

namespace vac {
namespace container {

/*!
 * \brief  Type for nodes of the intrusive map.
 *         This serves as a pair of key and value to be inserted in the map as a node.
 * \tparam key Type for key in the pair.
 * \tparam T Type of the value in the pair.
 */
template <typename key, typename T>
class IntrusiveMapNode {
 public:
  /*!
   * \brief Type of contained key.
   */
  using key_type = key;

  /*!
   * \brief Type of compare result.
   */
  using CompareType = int;

  /*!
   * \brief Constructor for a node without a map.
   */
  IntrusiveMapNode() = default;

  /*!
   * \brief Default copy constructor deleted.
   */
  IntrusiveMapNode(IntrusiveMapNode const&) = delete;

  /*!
   * \brief Default copy assignment operator deleted.
   */
  IntrusiveMapNode& operator=(IntrusiveMapNode const&) & = delete;

  /*!
   * \brief Default move constructor deleted.
   */
  IntrusiveMapNode(IntrusiveMapNode&&) = delete;

  /*!
   * \brief  Default move assignment operator deleted.
   */
  IntrusiveMapNode& operator=(IntrusiveMapNode&&) & = delete;

  /*!
   * \brief  Compare the keys of this element to another key.
   * \return Negative value if new key is less than the other current node key, zero if the both keys are equal,
   *         positive value if new key is greater than the other current node key.
   */
  virtual CompareType KeyCompare(key_type) const = 0;

  /*!
   * \brief  Compare the keys of this element to the key obtained from another element.
   *         This function is necessary as the IntrusiveMap class needs to compare IntrusiveMapNode objects. However, as
   *         it does not know how the IntrusiveMapNode stores its key, it cannot extract the key for comparison using
   *         KeyCompare(key_type).
   * \return Negative value if new key is less than the other current node key, zero if the both keys are equal,
   *         positive value if new key is greater than the other current node key.
   */
  virtual CompareType KeyCompare(T const&) const = 0;

  /*!
   * \brief Destructor that removes the node from a map, if it is contained in one.
   */
  virtual ~IntrusiveMapNode() { EraseFromMap(); }

  /*!
   * \brief Erase the node from a map.
   */
  void EraseFromMap() {
    // Both child nodes present.
    if ((left_ != nullptr) && (right_ != nullptr)) {
      // Find the node with maximum key value in the left sub tree.
      IntrusiveMapNode<key, T>* temp{FindMaxLeft()};

      // Erase temp from the map.
      temp->EraseNodeWithOneOrNoChild();

      // Adjust the pointers of the node and parent node.
      temp->SetLeft(left_);
      temp->SetRight(right_);
      temp->SetParent(parent_);

      if (parent_->Right() == this) {
        parent_->SetRight(temp);
      }
      if (parent_->Left() == this) {
        parent_->SetLeft(temp);
      }

      // Adjust the left and right parent pointers.
      if (left_ != nullptr) {
        left_->parent_ = temp;
      }
      // The sorted map node (except for the rightest node) will always contains the right hand child.
      right_->parent_ = temp;

      // Reset links to parent and child.
      left_ = nullptr;
      right_ = nullptr;
      parent_ = nullptr;

    } else {
      // If node has one or no child
      EraseNodeWithOneOrNoChild();
    }
  }

  /*!
   * \brief  Get the contained element.
   * \return The contained element.
   */
  T* GetSelf() {
    static_assert(std::is_base_of<IntrusiveMapNode<key, T>, T>::value, "T must derive from IntrusiveMapNode");
    return static_cast<T*>(this);
  }

  /*!
   * \brief  Get the contained element.
   * \return The contained element.
   */
  const T* GetSelf() const {
    static_assert(std::is_base_of<IntrusiveMapNode<key, T>, T>::value, "T must derive from IntrusiveMapNode");
    return static_cast<const T*>(this);
  }

  /*!
   * \brief  Returns the pointer to left node.
   * \return Pointer to the left child node.
   */
  IntrusiveMapNode* Left() { return left_; }

  /*!
   * \brief  Returns const pointer to left node.
   * \return Const pointer to the left child node.
   */
  IntrusiveMapNode const* Left() const { return left_; }

  /*!
   * \brief  Returns the pointer to right node.
   * \return Pointer to the right child node.
   */
  IntrusiveMapNode* Right() { return right_; }

  /*!
   * \brief  Returns const pointer to right node.
   * \return Const pointer to the right child node.
   */
  IntrusiveMapNode const* Right() const { return right_; }

  /*!
   * \brief  Returns the pointer to parent node.
   * \return Pointer to the parent node.
   */
  IntrusiveMapNode* Parent() { return parent_; }

  /*!
   * \brief  Returns const pointer to parent node.
   * \return Const pointer to the parent node.
   */
  IntrusiveMapNode const* Parent() const { return parent_; }

  /*!
   * \brief Set the left element.
   * \param new_elem Pointer to an IntrusiveMapNode.
   */
  void SetLeft(IntrusiveMapNode* new_elem) { left_ = new_elem; }

  /*!
   * \brief Set the right element.
   * \param new_elem Pointer to an IntrusiveMapNode.
   */
  void SetRight(IntrusiveMapNode* new_elem) { right_ = new_elem; }

  /*!
   * \brief Set the parent element.
   * \param parent Pointer to an IntrusiveMapNode.
   */
  void SetParent(IntrusiveMapNode* parent) { parent_ = parent; }

  /*!
   * \brief  Find min element from the right subtree.
   * \return Element from right subtree with minimum key or nullptr if the right subtree is empty.
   */
  IntrusiveMapNode const* FindMinRight() const noexcept {
    IntrusiveMapNode const* min_right{this};
    if (min_right->Right() == nullptr) {
      min_right = nullptr;
    } else {
      min_right = min_right->Right();
      /* VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect */
      while (min_right->Left() != nullptr) {
        min_right = min_right->Left();
      }
    }
    return min_right;
  }

  /*!
   * \brief  Find max element from the left subtree.
   * \return Element from left subtree with maximum key, or nullptr if the left subtree is emtpy.
   */
  IntrusiveMapNode const* FindMaxLeft() const noexcept {
    IntrusiveMapNode const* max_left{this};
    if (max_left->Left() == nullptr) {
      max_left = nullptr;
    } else {
      max_left = max_left->Left();
      /* VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect */
      while (max_left->Right() != nullptr) {
        max_left = max_left->Right();
      }
    }
    return max_left;
  }

  /*!
   * \brief  Find a parent node with immediate large key value.
   * \return Parent with larger key value or nullptr in case such a parent is not found.
   */
  IntrusiveMapNode const* FindLargerParent() const noexcept {
    IntrusiveMapNode const* drag{this};
    IntrusiveMapNode const* larger_parent{this};
    /* VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect */
    while ((larger_parent != nullptr) && (larger_parent->Left() != drag)) {
      // While loop terminates when reaching the end() element or when coming in to the parent from a left child.
      drag = larger_parent;
      larger_parent = larger_parent->Parent();
    }  // At this point, larger_parent is either the larger parent or end().
    return larger_parent;
  }

  /*!
   * \brief  Find a parent node with immediate smaller key value.
   * \return Parent with smaller key value or nullptr in case such a parent is not found.
   */
  IntrusiveMapNode const* FindSmallerParent() const noexcept {
    IntrusiveMapNode const* drag{this};
    IntrusiveMapNode const* smaller_parent{this};
    /* VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect */
    while ((smaller_parent != nullptr) && (smaller_parent->Right() != drag)) {
      // While loop terminates when reaching the end() element or when coming in to the parent from a right child
      drag = smaller_parent;
      smaller_parent = smaller_parent->Parent();
    }  // At this point, larger_parent is either the larger parent or end().
    return smaller_parent;
  }

  /* VECTOR Next Construct AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication */
  /*!
   * \brief  Find max element from the left subtree.
   * \return Element from left subtree with maximum key.
   */
  IntrusiveMapNode* FindMaxLeft() noexcept {
    return const_cast<IntrusiveMapNode*>(static_cast<IntrusiveMapNode const*>(this)->FindMaxLeft());
  }

  /* VECTOR Next Construct AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication */
  /*!
   * \brief  Find min element from the right subtree.
   * \return Element from right subtree with minimum key.
   */
  IntrusiveMapNode* FindMinRight() noexcept {
    return const_cast<IntrusiveMapNode*>(static_cast<IntrusiveMapNode const*>(this)->FindMinRight());
  }

  /* VECTOR Next Construct AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication */
  /*!
   * \brief  Find a parent node with immediate large key value.
   * \return Parent with larger key value or nullptr in case such a parent is not found.
   */
  IntrusiveMapNode* FindLargerParent() noexcept {
    return const_cast<IntrusiveMapNode*>(static_cast<IntrusiveMapNode const*>(this)->FindLargerParent());
  }

  /* VECTOR Next Construct AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication */
  /*!
   * \brief  Find a parent node with immediate smaller key value.
   * \return Parent with smaller key value or nullptr in case such a parent is not found.
   */
  IntrusiveMapNode* FindSmallerParent() noexcept {
    return const_cast<IntrusiveMapNode*>(static_cast<IntrusiveMapNode const*>(this)->FindSmallerParent());
  }

 private:
  /*!
   * \brief Erase the node from a map that has either no or exactly one child.
   */
  void EraseNodeWithOneOrNoChild() noexcept {
    // No or one child present

    assert(((left_ == nullptr) || (right_ == nullptr)) &&
           ("This function is intended only for deleting a node that has only one or no child." != nullptr));
    if (this->HasParent()) {
      if (parent_->right_ == this) {
        parent_->right_ = (left_ == nullptr) ? right_ : left_;
      }
      if (parent_->left_ == this) {
        parent_->left_ = (left_ == nullptr) ? right_ : left_;
      }
    }
    // Adjust the left and right parent pointers
    if (left_ != nullptr) {
      left_->parent_ = parent_;
    }
    if (right_ != nullptr) {
      right_->parent_ = parent_;
    }

    // Adjust the node pointers
    left_ = nullptr;
    right_ = nullptr;
    parent_ = nullptr;
  }

  /*!
   * \brief  Determines if the 'this' node has a parent node.
   * \return True if a parent node exists, else false.
   */
  bool HasParent() const { return parent_ != nullptr; }

  /*!
   * \brief Pointer to the left child node.
   */
  IntrusiveMapNode* left_{nullptr};

  /*!
   * \brief Pointer to the right child node.
   */
  IntrusiveMapNode* right_{nullptr};

  /*!
   * \brief Pointer to the parent node.
   */
  IntrusiveMapNode* parent_{nullptr};
};

/*!
 * \brief Type for end node of the intrusive map.
 */
template <typename Key, typename T>
class EndNode final : public IntrusiveMapNode<Key, T> {
 public:
  /*!
   * \brief Constructor.
   */
  using IntrusiveMapNode<Key, T>::IntrusiveMapNode;

  /*!
   * \brief Type of compare result.
   */
  using CompareType = typename IntrusiveMapNode<Key, T>::CompareType;

  /*!
   * \brief  Compares the  key value with the node current node value.
   * \return Always returns 1 because this is the root node with only left subtree so the key is considered to be
   *         always greater then any other key in the map.
   */
  CompareType KeyCompare(Key) const final { return 1; }

  /*!
   * \brief  Compares the  key value with the node current node value.
   * \return Always returns 1 because this is the root node with only left subtree so the key is considered to be
   *         always greater then any other key in the map.
   */
  CompareType KeyCompare(T const&) const final { return 1; }
};

/*!
 * \brief Iterator for this IntrusiveMap.
 * \trace CREQ-160859
 */
template <typename key, typename T>
class IntrusiveMapIterator final {
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

  /*!
   * \brief Constructor for an iterator.
   * \param node The Map from which to construct the iterator.
   */
  explicit IntrusiveMapIterator(IntrusiveMapNode<key, T>* node) : node_(node) {}

  /*!
   * \brief  Return a reference to the contained element.
   * \return A reference to the element type.
   */
  T& operator*() { return *(this->GetMapNode()->GetSelf()); }

  /*!
   * \brief  Return a reference to the contained element.
   * \return A const reference to the element type.
   */
  T const& operator*() const { return *(this->GetMapNode()->GetSelf()); }

  /*!
   * \brief  Get the map node .
   * \return The contained pointer to the map node.
   */
  IntrusiveMapNode<key, T>* GetMapNode() { return node_; }

  /*!
   * \brief  Get the map node .
   * \return The contained pointer to the map node.
   */
  const IntrusiveMapNode<key, T>* GetMapNode() const { return node_; }

  /*!
   * \brief  Compare two iterators for equality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to the same map node.
   */
  bool operator==(IntrusiveMapIterator const& other) const { return node_ == other.node_; }

  /*!
   * \brief  Compare two iterators for inequality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to different map nodes.
   */
  bool operator!=(IntrusiveMapIterator const& other) const { return node_ != other.node_; }

  /*!
   * \brief  Increment the iterator by one element.
   * \return IntrusiveMapIterator to the element with immediate higher key value.
   */
  IntrusiveMapIterator& operator++() {
    IntrusiveMapNode<key, T>* temp_elem{node_};
    if (node_ != nullptr) {
      temp_elem = node_->FindMinRight();

      // If Min right node not found.
      if (temp_elem == nullptr) {
        temp_elem = node_->FindLargerParent();
      }
    }
    if (temp_elem != nullptr) {
      // Avoid leaving the tree, remain on the end() element.
      node_ = temp_elem;
    }
    return *this;
  }

  /*!
   * \brief  Decrement the iterator by one element.
   * \return IntrusiveMapIterator to the element with an immediate lower key value.
   */
  IntrusiveMapIterator& operator--() {
    IntrusiveMapNode<key, T>* temp_elem{node_};
    if (node_ != nullptr) {
      temp_elem = node_->FindMaxLeft();

      // If Max Left node not found.
      if (temp_elem == nullptr) {
        temp_elem = node_->FindSmallerParent();
      }
    }
    node_ = temp_elem;
    return *this;
  }

 private:
  /*!
   * \brief Pointer to the map element.
   */
  IntrusiveMapNode<key, T>* node_;
};

/*!
 * \brief Const Iterator for this IntrusiveMap.
 */
template <typename key, typename T>
class ConstIntrusiveMapIterator final {
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
   * \brief Constructor for an iterator.
   * \param node The Map from which to construct the iterator.
   */
  explicit ConstIntrusiveMapIterator(const IntrusiveMapNode<key, T>* node) : node_(node) {}

  /*!
   * \brief  Return a reference to the contained element.
   * \return A const reference to the element type.
   */
  reference operator*() const { return *(this->GetMapNode()->GetSelf()); }

  /*!
   * \brief  Get the map node .
   * \return The contained pointer to the map node.
   */
  const IntrusiveMapNode<key, T>* GetMapNode() const { return node_; }

  /*!
   * \brief  Compare two iterators for equality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to the same map node.
   */
  bool operator==(ConstIntrusiveMapIterator const& other) const { return node_ == other.node_; }

  /*!
   * \brief  Compare two iterators for inequality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to different map nodes.
   */
  bool operator!=(ConstIntrusiveMapIterator const& other) const { return node_ != other.node_; }

  /*!
   * \brief  Increment the iterator by one element.
   * \return IntrusiveMapIterator to the element with immediate higher key value.
   */
  ConstIntrusiveMapIterator& operator++() {
    const IntrusiveMapNode<key, T>* temp_elem{node_};
    if (node_ != nullptr) {
      temp_elem = node_->FindMinRight();

      // If Min right node not found.
      if (temp_elem == nullptr) {
        temp_elem = node_->FindLargerParent();
      }
    }
    node_ = temp_elem;
    return *this;
  }

  /*!
   * \brief  Decrement the iterator by one element.
   * \return IntrusiveMapIterator to the element with an immediate lower key value.
   */
  ConstIntrusiveMapIterator& operator--() {
    const IntrusiveMapNode<key, T>* temp_elem{node_};
    if (node_ != nullptr) {
      temp_elem = node_->FindMaxLeft();

      // If Max Left node not found.
      if (temp_elem == nullptr) {
        temp_elem = node_->FindSmallerParent();
      }
    }
    node_ = temp_elem;
    return *this;
  }

 private:
  /*!
   * \brief Pointer to the map element.
   */
  const IntrusiveMapNode<key, T>* node_;
};

/*!
 * \brief Class to implement an IntrusiveMap.
 * \trace CREQ-158591
 */
template <typename key, typename T>
class IntrusiveMap final {
 public:
  /*!
   * \brief Typedef for the contained element.
   */
  using value_type = T;

  /*!
   * \brief Typedef for the iterator type of this map.
   */
  using iterator = IntrusiveMapIterator<key, T>;

  /*!
   * \brief Typedef for the iterator type of this map.
   */
  using const_iterator = ConstIntrusiveMapIterator<key, T>;

  /*!
   * \brief Type of compare result.
   */
  using CompareType = typename IntrusiveMapNode<key, T>::CompareType;

  /*!
   * \brief Constructor.
   */
  IntrusiveMap() : map_() { map_.SetParent(nullptr); }

  /*!
   * \brief Destructor.
   */
  ~IntrusiveMap() {
    // Release all elements from the map starting from the smallest key member.
    while (!empty()) {
      erase(begin());
    }
  }

  /*!
   * \brief Deleted copy constructor.
   */
  IntrusiveMap(IntrusiveMap const&) = delete;
  /*!
   * \brief Deleted move constructor.
   */
  IntrusiveMap(IntrusiveMap&&) = delete;
  /*!
   * \brief Deleted copy assignent.
   */
  IntrusiveMap& operator=(IntrusiveMap const&) & = delete;
  /*!
   * \brief Deleted move assignent.
   */
  IntrusiveMap& operator=(IntrusiveMap&&) & = delete;

  /*!
   * \brief  Remove an element from map.
   * \param  erase_key Key to be erased.
   * \return Number of elements removed.
   */
  std::size_t erase(key const& erase_key) {
    std::size_t deleted_count{0};
    iterator itr{find(erase_key)};

    /* VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect */
    if ((map_.Left() != nullptr) && (itr != end())) {
      IntrusiveMapNode<key, T>* elem{itr.GetMapNode()};
      // Erase the node.
      elem->EraseFromMap();
      deleted_count++;
    }
    return deleted_count;
  }

  /*!
   * \brief Remove an element from map.
   * \param elem Iterator to the element to be erased.
   */
  void erase(iterator elem) {
    /* VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect */
    if ((!empty()) && (elem != end())) {
      if (elem.GetMapNode() != nullptr) {
        elem.GetMapNode()->EraseFromMap();
      }
    }
  }

  /*!
   * \brief  Determine whether the map is currently empty.
   * \return True if the map is empty. False if the map has at least one element.
   */
  bool empty() const { return (map_.Left() == nullptr); }

  /*!
   * \brief  Insert a new node (key value pair) into the map at appropriate position.
   * \param  node Element to be inserted in the map.
   * \return Pair of iterator and boolean that indicates if the item was added (true)
   *         or an item with the same key already exists (false).
   */
  std::pair<iterator, bool> insert(IntrusiveMapNode<key, T>* node) {
    bool is_inserted{false};
    if (map_.Left() == nullptr) {
      map_.SetLeft(node);
      if (node != nullptr) {
        node->SetParent(&map_);
        is_inserted = true;
      }
    } else {
      if (node != nullptr) {
        T const* self{node->GetSelf()};
        IntrusiveMapNode<key, T>* temp_node{SearchNode(*self)};
        CompareType const result{temp_node->KeyCompare(*self)};

        if (result > 0) {
          temp_node->SetRight(node);
          node->SetParent(temp_node);
          is_inserted = true;
        } else if (result < 0) {
          temp_node->SetLeft(node);
          node->SetParent(temp_node);
          is_inserted = true;
        } else {
          // Get the node that prevented the insertion.
          node = temp_node;
        }
      }
    }
    return std::make_pair(iterator(node), is_inserted);
  }

  /*!
   * \brief  Find node in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a end_ node.
   */
  iterator find(key const& find_key) noexcept {
    iterator ret_value{end()};
    IntrusiveMapNode<key, T>* node{SearchNode(find_key)};
    if ((node != nullptr) && (node->KeyCompare(find_key) == 0)) {
      ret_value = iterator(node);
    }
    return ret_value;
  }

  /*!
   * \brief  Find node in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a end_ node.
   */
  const_iterator find(key const& find_key) const noexcept {
    const_iterator ret_value{cend()};
    IntrusiveMapNode<key, T> const* node{SearchNode(find_key)};
    if ((node != nullptr) && (node->KeyCompare(find_key) == 0)) {
      ret_value = const_iterator(node);
    }
    return ret_value;
  }

  /*!
   * \brief  Get iterator to smallest key node.
   * \return Iterator to Intrusive map.
   */
  iterator begin() {
    iterator ret_value{end()};
    if (!empty()) {
      IntrusiveMapNode<key, T>* node{map_.Left()};
      while (node->Left() != nullptr) {
        node = node->Left();
      }
      ret_value = iterator(node);
    }
    return ret_value;
  }

  /*!
   * \brief  Get const iterator to smallest key node.
   * \return Const iterator to Intrusive map.
   */
  const_iterator cbegin() const {
    const_iterator ret_value{cend()};
    if (!empty()) {
      IntrusiveMapNode<key, T> const* node{map_.Left()};
      while (node->Left() != nullptr) {
        node = node->Left();
      }
      ret_value = const_iterator(node);
    }
    return ret_value;
  }

  /*!
   * \brief  Get iterator to end element.
   * \return Iterator to end element.
   */
  iterator end() { return iterator(&map_); }

  /*!
   * \brief  Get iterator to end element.
   * \return Iterator to end element.
   */
  const_iterator cend() const { return const_iterator(&map_); }

 private:
  /*!
   * \brief  Search for a node position in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a node just previous to the matching key.
   */
  template <class KeyOrNode>
  const IntrusiveMapNode<key, T>* SearchNode(KeyOrNode const& find_key) const noexcept {
    IntrusiveMapNode<key, T> const* temp{map_.Left()};
    IntrusiveMapNode<key, T> const* prev{nullptr};
    while (temp != nullptr) {
      prev = temp;
      CompareType const result{temp->GetSelf()->KeyCompare(find_key)};
      if (result > 0) {
        temp = temp->Right();
      } else if (result < 0) {
        temp = temp->Left();
      } else {
        break;
      }
    }
    return prev;
  }

  /* VECTOR Next Construct AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication */
  /*!
   * \brief  Search for a node position in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a node just previous to the matching key.
   */
  template <class KeyOrNode>
  IntrusiveMapNode<key, T>* SearchNode(KeyOrNode const& find_key) noexcept {
    return const_cast<IntrusiveMapNode<key, T>*>(const_cast<IntrusiveMap const*>(this)->SearchNode(find_key));
  }

  /*!
   * \brief Pointer to the map.
   *        Map_ is the root node and the actual map starts from the map_.left. map_.right will always be nullptr.
   *        For an empty map the map_.left will be nullptr.
   */
  EndNode<key, T> map_;

  FRIEND_TEST(IntrusiveMap, IteratorOperationsIncrementRightSubTree);
  FRIEND_TEST(IntrusiveMap, IteratorOperationsIncrementLeftSubTree);
};

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_INTRUSIVE_MAP_H_
