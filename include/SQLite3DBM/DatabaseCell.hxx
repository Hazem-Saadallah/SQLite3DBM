#pragma once

#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <sqlite3.h>
#include <SQLite3DBM/DatabaseManager.hxx>
#include <SQLite3DBM/CellDescriptor.hxx>

class _DatabaseCell {
private:
  _DatabaseManager::SqlCell_t m_Data;
  _DatabaseManager::SqlType_t m_Type = _DatabaseManager::SqlType_t();
  std::shared_ptr<_CellDescriptor> m_CellDescription;

public:
  _DatabaseCell(_DatabaseManager::SqlCell_t&& data, const _DatabaseManager::SqlType_t& type, const std::shared_ptr<_CellDescriptor>& cell_descriptor);
  ~_DatabaseCell() = default;

  _DatabaseCell(_DatabaseCell && other) noexcept;
  _DatabaseCell &operator=(_DatabaseCell && other) noexcept;

  _DatabaseCell(const _DatabaseCell & other) = delete;
  _DatabaseCell &operator=(const _DatabaseCell &) = delete;

  [[nodiscard]] std::shared_ptr<_CellDescriptor> get_cell_descriptor() const;
  [[nodiscard]] std::string get_name() const;

  [[nodiscard]] _DatabaseManager::SqlType_t get_type() const;
  [[nodiscard]] std::uint8_t get_index() const;

  [[nodiscard]] const std::optional<std::int64_t> as_integer() const;
  [[nodiscard]] const std::optional<std::double_t> as_double() const;
  [[nodiscard]] const std::optional<std::string> as_string() const;
  [[nodiscard]] const std::optional<_DatabaseManager::SqlBlob_t> as_blob() const;

  [[nodiscard]] bool is_null() const;
  [[nodiscard]] bool is_integer() const;
  [[nodiscard]] bool is_double() const;
  [[nodiscard]] bool is_string() const;
  [[nodiscard]] bool is_blob() const;
};

namespace _DatabaseManager {
  typedef BiMap<std::uint8_t, std::string> RowDescription_t;
  typedef std::unordered_map<std::uint8_t, _DatabaseCell> RowData_t;
  typedef std::unordered_map<std::uint8_t, std::shared_ptr<_CellDescriptor>> CellDescriptorContainer_t;
}
