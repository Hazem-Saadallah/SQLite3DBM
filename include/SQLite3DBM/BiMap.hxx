#pragma once

#include <cstddef>
#include <utility>
#include <type_traits>
#include <unordered_map>
#include <functional>
#include <SQLite3DBM/BiMapIterator.hxx>

template <typename T, typename=void> class is_hashable : public std::false_type {};
template <typename T> class is_hashable<T, std::void_t<decltype(std::hash<T>{}(std::declval<T>()))>> : public std::true_type {};

template <typename K1, typename K2> class BiMap {
  static_assert(!std::is_same_v<K1, K2>, "BiMap types K1 and K2 must be different");
  static_assert(is_hashable<K1>::value, "Type K1 is not hashable");
  static_assert(is_hashable<K2>::value, "Type K2 is not hashable");

private:
  std::unordered_map<K1, K2> m_LeftToRight;
  std::unordered_map<K2, K1> m_RightToLeft;
  typedef typename std::unordered_map<K1, K2>::iterator Left2RightItr;
  typedef typename std::unordered_map<K2, K1>::iterator Right2LeftItr;
  typedef typename std::unordered_map<K1, K2>::const_iterator Left2RightConstItr;
  typedef typename std::unordered_map<K2, K1>::const_iterator Right2LeftConstItr;

public:
  BiMap() = default;
  ~BiMap() = default;

  BiMap(BiMap&& other) noexcept
  : m_LeftToRight(std::move(other.m_LeftToRight)), m_RightToLeft(std::move(other.m_RightToLeft)) { }

  BiMap &operator=(BiMap&& other) noexcept {
    if(this == &other) return *this;
    m_LeftToRight = std::move(other.m_LeftToRight);
    m_RightToLeft = std::move(other.m_RightToLeft);
    return *this;
  }

  BiMap(const BiMap &) = delete;
  BiMap &operator=(const BiMap &) = delete;

  void insert(const K1& k1, const K2& k2) {
    if(Left2RightItr itr = m_LeftToRight.find(k1); itr != m_LeftToRight.end()) m_RightToLeft.erase(itr->second);
    if(Right2LeftItr itr = m_RightToLeft.find(k2); itr != m_RightToLeft.end()) m_LeftToRight.erase(itr->second);
    m_LeftToRight[k1] = k2;
    m_RightToLeft[k2] = k1;
  }

  const K2& get(const K1& key) const { return m_LeftToRight.at(key); }
  const K1& get(const K2& key) const { return m_RightToLeft.at(key); }

  const K2* try_get(const K1& key) const {
    Left2RightConstItr itr = m_LeftToRight.find(key);
    return (itr != m_LeftToRight.end())? &itr->second : nullptr;
  }
    
  const K1* try_get(const K2& key) const {
    Right2LeftConstItr itr = m_RightToLeft.find(key);
    return (itr != m_RightToLeft.end())? &itr->second : nullptr;
  }

  bool contains(const K1& key) const { return m_LeftToRight.count(key); }
  bool contains(const K2& key) const { return m_RightToLeft.count(key); }

  void erase(const K1& key) {
    Left2RightItr itr = m_LeftToRight.find(key);
    if(itr == m_LeftToRight.end()) return;
    m_RightToLeft.erase(itr->second);
    m_LeftToRight.erase(itr);
  }

  void erase(const K2& key) {
    Right2LeftItr itr = m_RightToLeft.find(key);
    if(itr == m_RightToLeft.end()) return;
    m_LeftToRight.erase(itr->second);
    m_RightToLeft.erase(itr);
  }

  std::size_t size() const { return m_LeftToRight.size(); }
  void clear() {
    m_LeftToRight.clear();
    m_RightToLeft.clear();
  }

  using iterator = _BiMapIterator<K1, K2, false>;
  using const_iterator = _BiMapIterator<K1, K2, true>;

  iterator begin() { return iterator(m_LeftToRight.begin()); }
  iterator end() { return iterator(m_LeftToRight.end()); }

  const_iterator begin() const { return const_iterator(m_LeftToRight.begin()); }
  const_iterator end() const { return const_iterator(m_LeftToRight.end()); }

  const_iterator cbegin() const { return const_iterator(m_LeftToRight.begin()); }
  const_iterator cend() const { return const_iterator(m_LeftToRight.end()); }
};
