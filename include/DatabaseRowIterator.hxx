// #pragma once

#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <DatabaseCell.hxx>

template <bool IsConst> class _DatabaseRowIterator {
public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::conditional_t<IsConst, const _DatabaseCell, _DatabaseCell>;
  using pointer = value_type*;
  using reference = value_type&;
  using MapIteratorType = std::conditional_t<IsConst,
                          _DatabaseManager::RowData_t::const_iterator,
                          _DatabaseManager::RowData_t::iterator>;

private:
  MapIteratorType m_Ptr;

public:
  _DatabaseRowIterator() = default;
  explicit _DatabaseRowIterator(MapIteratorType ptr) : m_Ptr(ptr) {}

  template <bool WasConst, typename = std::enable_if_t<IsConst && !WasConst>>
  _DatabaseRowIterator(const _DatabaseRowIterator<WasConst>& other) : m_Ptr(other.m_Ptr) {}

  _DatabaseRowIterator& operator++() {
    ++m_Ptr;
    return *this;
  }

  _DatabaseRowIterator operator++(int) {
    _DatabaseRowIterator temp = *this;
    ++(*this);
    return temp;
  }

  reference operator*() const { return m_Ptr->second; }
  pointer operator->() const { return &(m_Ptr->second); }
  friend bool operator==(const _DatabaseRowIterator& a, const _DatabaseRowIterator& b) { return a.m_Ptr == b.m_Ptr; }
  friend bool operator!=(const _DatabaseRowIterator& a, const _DatabaseRowIterator& b) { return a.m_Ptr != b.m_Ptr; }
  
  friend class _DatabaseRowIterator<!IsConst>;
};
