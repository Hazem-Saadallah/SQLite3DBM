#pragma once

#include <cmath>
#include <string>
#include <variant>
#include <cstdint>
#include <sqlite3.h>
#include <SQLite3DBM/DatabaseManager.hxx>

class _Sqlite3Binder {
public:
  std::uint8_t m_Index;
  sqlite3_stmt* m_Statement;

  [[nodiscard]] std::int32_t operator()(std::monostate) const;
  [[nodiscard]] std::int32_t operator()(std::int64_t val) const;
  [[nodiscard]] std::int32_t operator()(std::double_t val) const;
  [[nodiscard]] std::int32_t operator()(const std::string& val) const;
  [[nodiscard]] std::int32_t operator()(const _DatabaseManager::SqlBlob_t& blob) const;
};
