#include <cassert>
#include <DatabaseRow.hxx>
#include <CellDescriptor.hxx>

_DatabaseRow::_DatabaseRow(const std::shared_ptr<_DatabaseManager::RowDescription_t>& row_description, _DatabaseManager::RowData_t&& row_data)
: m_Data(std::move(row_data)), m_RowDescription(row_description) {
  if (m_RowDescription->size() != m_Data.size()) throw std::length_error("Mismatch between row description and data count.");
}

_DatabaseRow::_DatabaseRow(_DatabaseRow&& other) noexcept
: m_Data(std::move(other.m_Data)), m_RowDescription(std::move(other.m_RowDescription)) { }

_DatabaseRow &_DatabaseRow::operator=(_DatabaseRow&& other) noexcept {
  if(this == &other) return *this;
  m_Data = std::move(other.m_Data);
  m_RowDescription = std::move(other.m_RowDescription);
  return *this;
}

const _DatabaseCell* _DatabaseRow::get_cell_by_index(std::uint8_t index) const {
  if(index >= size()) return nullptr;
  return &m_Data.at(index);
}

const _DatabaseCell* _DatabaseRow::get_cell_by_name(std::string name) const {
  const std::uint8_t* index = m_RowDescription->try_get(name);
  return index? &m_Data.at(*index) : nullptr;
}

bool _DatabaseRow::is_row() const { return m_Data.size() != 0; }

std::size_t _DatabaseRow::size() const { return m_Data.size(); }

_DatabaseRow::iterator _DatabaseRow::begin() { return iterator(m_Data.begin()); }
_DatabaseRow::iterator _DatabaseRow::end() { return iterator(m_Data.end()); }

_DatabaseRow::const_iterator _DatabaseRow::begin() const { return const_iterator(m_Data.begin()); }
_DatabaseRow::const_iterator _DatabaseRow::end() const { return const_iterator(m_Data.end()); }

_DatabaseRow::const_iterator _DatabaseRow::cbegin() const { return const_iterator(m_Data.cbegin()); }
_DatabaseRow::const_iterator _DatabaseRow::cend() const { return const_iterator(m_Data.cend()); }
