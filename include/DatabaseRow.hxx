#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <sqlite3.h>
#include <DatabaseCell.hxx>
#include <DatabaseManager.hxx>
#include <CellDescriptor.hxx>
#include <DatabaseRowIterator.hxx>

class _DatabaseRow {
private:
  _DatabaseManager::RowData_t m_Data; 
  std::shared_ptr<_DatabaseManager::RowDescription_t> m_RowDescription;

public:
  _DatabaseRow(const std::shared_ptr<_DatabaseManager::RowDescription_t>& row_description, _DatabaseManager::RowData_t&& row_data);
  _DatabaseRow() = default;
  ~_DatabaseRow() = default;

  _DatabaseRow(_DatabaseRow && other) noexcept;
  _DatabaseRow &operator=(_DatabaseRow && other) noexcept;

  _DatabaseRow(const _DatabaseRow &) = delete;
  _DatabaseRow &operator=(const _DatabaseRow &) = delete;

  [[nodiscard]] const _DatabaseCell* get_cell_by_index(std::uint8_t index) const;
  [[nodiscard]] const _DatabaseCell* get_cell_by_name(std::string name) const;
  [[nodiscard]] bool is_row() const;
  [[nodiscard]] std::size_t size() const;

  using iterator = _DatabaseRowIterator<false>;
  using const_iterator = _DatabaseRowIterator<true>;

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;
  
  const_iterator cbegin() const;
  const_iterator cend() const;
};
