#pragma once

#include <iterator>
#include <type_traits>
#include <unordered_map>

template <typename K1, typename K2, bool IsConst> class _BiMapIterator {
  friend class _BiMapIterator<K1, K2, !IsConst>;

public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::conditional_t<IsConst, const std::pair<const K1, K2>, std::pair<const K1, K2>>;
  using pointer = const value_type*;
  using reference = const value_type&;
  using MapIteratorType = std::conditional_t<IsConst,
                          typename std::unordered_map<K1, K2>::const_iterator,
                          typename std::unordered_map<K1, K2>::iterator>;

private:
  MapIteratorType m_Ptr;

public:
  _BiMapIterator() = default;
  explicit _BiMapIterator(MapIteratorType ptr) : m_Ptr(ptr) {}

  template <bool WasConst, typename = std::enable_if_t<IsConst && !WasConst>>
  _BiMapIterator(const _BiMapIterator<K1, K2, WasConst>& other) : m_Ptr(other.m_Ptr) {}

  _BiMapIterator& operator++() {
    ++m_Ptr;
    return *this;
  }

  _BiMapIterator operator++(int) {
    _BiMapIterator temp = *this;
    ++(*this);
    return temp;
  }

  reference operator*() const { return m_Ptr.operator*(); }
  pointer operator->() const { return m_Ptr.operator->(); }
  friend bool operator==(const _BiMapIterator& a, const _BiMapIterator& b) { return a.m_Ptr == b.m_Ptr; }
  friend bool operator!=(const _BiMapIterator& a, const _BiMapIterator& b) { return a.m_Ptr != b.m_Ptr; }
};
