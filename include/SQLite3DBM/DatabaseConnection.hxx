#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <sqlite3.h>
#include <vector>
#include <bitset>
#include <SQLite3DBM/DatabaseManager.hxx>

class _DatabaseConnection {
friend class _DatabaseQuery;

private:
  enum class Flag : std::uint8_t {
    DATABASE_INIT=0,
  };

  sqlite3 *m_Handle = nullptr;
  std::bitset<8> m_StateFlagsArray;
  std::vector<_DatabaseManager::Error_t> m_Error;
  std::vector<_DatabaseManager::Sqlite3Error_t> m_Sqlite3Error;

  bool check_flag(Flag flag);
  void set_flag(Flag flag);
  void clear_flag(Flag flag);

  void report_error_status(_DatabaseManager::ErrorCode_t error_code, const _DatabaseManager::ErrorMsg_t& error_msg);
  void report_sqlite3_error_status(_DatabaseManager::Sqlite3ErrorCode_t error_code);
  void report_external_sqlite3_error_status(_DatabaseManager::Sqlite3ErrorCode_t error_code);

  sqlite3* get_handle();

  std::int32_t close_database();

public:
  [[nodiscard]] bool is_initialized();
  [[nodiscard]] std::int64_t get_last_insert_rowid();

  // NOTE: Add error handling for empty error vectors
  [[nodiscard]] _DatabaseManager::Error_t get_last_error();
  [[nodiscard]] _DatabaseManager::ErrorCode_t get_last_error_code();
  [[nodiscard]] _DatabaseManager::ErrorMsg_t get_last_error_msg();
  [[nodiscard]] _DatabaseManager::Sqlite3Error_t get_last_sqlite3_error();
  [[nodiscard]] _DatabaseManager::Sqlite3ErrorCode_t get_last_sqlite3_error_code();
  [[nodiscard]] _DatabaseManager::Sqlite3ErrorMsg_t get_last_sqlite3_error_msg();

// private:
  _DatabaseConnection(const std::string& db_name);

public:
  ~_DatabaseConnection();
  _DatabaseConnection(_DatabaseConnection && other) noexcept;
  _DatabaseConnection &operator=(_DatabaseConnection && other) noexcept;
  _DatabaseConnection(const _DatabaseConnection & other) = delete;
  _DatabaseConnection &operator=(const _DatabaseConnection & other) = delete;

  static std::shared_ptr<_DatabaseConnection> Create(const std::string& db_name);
};
