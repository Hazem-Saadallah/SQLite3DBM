#pragma once

#include <bitset>
#include <memory>
#include <vector>
#include <cstdint>
#include <sqlite3.h>
#include <DatabaseConnection.hxx>
#include <DatabaseManager.hxx>
#include <DatabaseRow.hxx>
#include <DatabaseCell.hxx>

class _StatementDeleter {
public:
  void operator()(sqlite3_stmt* stmt) const;
};

class _DatabaseQuery {
private:
  enum class Flag : std::uint8_t {
    STMT_PREP=0,
    STMT_BIND,
    STMT_STEP,
    CELL_DESCRIPTION_PREP,
    ROW_DESCRIPTION_PREP,
  };

  std::bitset<8> m_StateFlagsArray;
  std::shared_ptr<_DatabaseConnection> m_Connection;
  std::shared_ptr<sqlite3_stmt> m_Statement; // NOTE: Covert back into regular reference (Use Unique pointer to utilize the deleterc 'StatementDeleter' or regular reference and update the destructor)
  std::vector<_DatabaseManager::Error_t> m_Error;
  std::vector<_DatabaseManager::Sqlite3Error_t> m_Sqlite3Error;

  _DatabaseManager::CellDescriptorContainer_t m_CellDescriptors;
  std::shared_ptr<_DatabaseManager::RowDescription_t> m_RowDescription;
  _DatabaseRow m_Row;

  bool check_flag(Flag flag);
  void set_flag(Flag flag);
  void clear_flag(Flag flag);

  void report_error_status(_DatabaseManager::ErrorCode_t error_code, const _DatabaseManager::ErrorMsg_t& error_msg);
  void report_sqlite3_error_status(_DatabaseManager::Sqlite3ErrorCode_t error_code);

  std::string read_file(const std::string& file_path);
  void prepare_row_description();
  void prepare_cells_descriptions();

public:
  [[nodiscard]] bool is_prepared();

  void bind_param(std::uint8_t index, const _DatabaseManager::SqlParam_t& param);
  void bind_params(const std::vector<_DatabaseManager::SqlParam_t>& params); // NOTE: Replace with an unordered_map (or any similar DS) for complete parameters verification

  [[nodiscard]] std::shared_ptr<sqlite3_stmt> get_statement_handle() const;

  [[nodiscard]] std::int8_t get_param_count();
  [[nodiscard]] std::string get_param_name(std::uint8_t index);

  [[nodiscard]] _DatabaseManager::Error_t get_last_error();
  [[nodiscard]] _DatabaseManager::ErrorCode_t get_last_error_code();
  [[nodiscard]] _DatabaseManager::ErrorMsg_t get_last_error_msg();
  [[nodiscard]] _DatabaseManager::Sqlite3Error_t get_last_sqlite3_error();
  [[nodiscard]] _DatabaseManager::Sqlite3ErrorCode_t get_last_sqlite3_error_code();
  [[nodiscard]] _DatabaseManager::Sqlite3ErrorMsg_t get_last_sqlite3_error_msg();

  std::int32_t prepare_statement_from_str(const std::string& stmt_str, const std::vector<std::uint32_t>& flags={});
  std::int32_t prepare_statement_from_file(const std::string& file_path, const std::vector<std::uint32_t>& flags={});

  std::int32_t step();
  std::int32_t exec_from_str(const std::string& query);
  std::int32_t exec_from_file(const std::string& file_path);

  [[nodiscard]] const _DatabaseRow get_current_row();
  [[nodiscard]] const _DatabaseCell* get_cell_by_name(std::string name);
  [[nodiscard]] const _DatabaseCell* get_cell_by_index(std::uint8_t index);
  [[nodiscard]] bool is_current_row() const;
  [[nodiscard]] std::size_t current_row_size() const;

  _DatabaseQuery(const std::shared_ptr<_DatabaseConnection>& connection);
  ~_DatabaseQuery() = default;

  _DatabaseQuery(_DatabaseQuery && other) noexcept;
  _DatabaseQuery &operator=(_DatabaseQuery && other) noexcept;

  _DatabaseQuery(const _DatabaseQuery &) = delete;
  _DatabaseQuery &operator=(const _DatabaseQuery &) = delete;
};
