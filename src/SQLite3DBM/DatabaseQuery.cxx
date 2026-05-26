#include <format>
#include <memory>
#include <fstream>
#include <numeric>
#include <utility>
#include <filesystem>
#include <sqlite3.h>
#include <SQLite3DBM/ErrorMsg.hxx>
#include <SQLite3DBM/Sqlite3Binder.hxx>
#include <SQLite3DBM/DatabaseQuery.hxx>
#include <SQLite3DBM/DatabaseManager.hxx>
#include <SQLite3DBM/DatabaseCell.hxx>

#include <iostream>

void _StatementDeleter::operator()(sqlite3_stmt* stmt) const {
  if (stmt) sqlite3_finalize(stmt);
}

_DatabaseQuery::_DatabaseQuery(const std::shared_ptr<_DatabaseConnection>& connection) : m_Connection(std::move(connection)) { }

_DatabaseQuery::_DatabaseQuery(_DatabaseQuery && other) noexcept
  : m_StateFlagsArray(std::move(other.m_StateFlagsArray)), 
  m_Connection(std::move(other.m_Connection)),
  m_Statement(std::move(other.m_Statement)),
  m_Error(std::move(other.m_Error)),
  m_Sqlite3Error(std::move(other.m_Sqlite3Error)),
  m_RowDescription(std::move(other.m_RowDescription)),
  m_Row(std::move(other.m_Row))
{ other.m_StateFlagsArray.reset(); }

_DatabaseQuery &_DatabaseQuery::operator=(_DatabaseQuery && other) noexcept {
  if(this == &other) return *this;

  m_StateFlagsArray = std::move(other.m_StateFlagsArray);
  m_Connection = std::move(other.m_Connection);
  m_Statement = std::move(other.m_Statement);
  m_Error = std::move(other.m_Error);
  m_Sqlite3Error = std::move(other.m_Sqlite3Error);
  m_RowDescription = std::move(other.m_RowDescription);
  m_Row = std::move(other.m_Row);

  other.m_StateFlagsArray.reset();
  return *this;
}

void _DatabaseQuery::report_error_status(_DatabaseManager::ErrorCode_t error_code, const _DatabaseManager::ErrorMsg_t& error_msg) {
  m_Error.push_back(_DatabaseManager::Error_t(error_code, error_msg));
  m_Connection->report_error_status(error_code, error_msg);
}

void _DatabaseQuery::report_sqlite3_error_status(_DatabaseManager::Sqlite3ErrorCode_t error_code) {
  m_Error.push_back(_DatabaseManager::Error_t(_DatabaseManager::ErrorCode_t::OK, ErrorMsg::sqlite3_internal_error));
  m_Sqlite3Error.push_back(_DatabaseManager::Sqlite3Error_t(error_code, std::string(sqlite3_errmsg(m_Connection->get_handle()))));
  m_Connection->report_external_sqlite3_error_status(error_code);
}

std::string _DatabaseQuery::read_file(const std::string& file_path) {
  if(!std::filesystem::exists(file_path)) {
    report_error_status(_DatabaseManager::ErrorCode_t::QUERY_FILE_NOT_FOUND, std::format(ErrorMsg::file_not_found, file_path));
    return std::string();
  }

  std::ifstream file(file_path);
  if (!file.is_open()) {
    report_error_status(_DatabaseManager::ErrorCode_t::QUERY_FILE_FAILED_OPEN, std::format(ErrorMsg::file_not_open, file_path));
    return std::string();
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  report_error_status(_DatabaseManager::ErrorCode_t::OK, std::format(ErrorMsg::file_read_success, file_path));
  return buffer.str();
}

std::int32_t _DatabaseQuery::prepare_statement_from_str(const std::string& stmt_str, const std::vector<std::uint32_t>& flags) {
  sqlite3_stmt* prep_ptr = nullptr;
  std::int32_t rc = sqlite3_prepare_v3(m_Connection->get_handle(), stmt_str.c_str(), -1, std::accumulate(flags.begin(), flags.end(), 0, std::bit_or<std::uint32_t>()), &prep_ptr, NULL);
  report_sqlite3_error_status(rc);
  if(rc == SQLITE_OK) {
    m_CellDescriptors.clear();
    m_Statement.reset(prep_ptr, _StatementDeleter());
    set_flag(Flag::STMT_PREP);
    prepare_row_description();
    clear_flag(Flag::CELL_DESCRIPTION_PREP);
  }
  return rc;
}

void _DatabaseQuery::prepare_row_description() {
  if(!m_Statement) {
    report_error_status(_DatabaseManager::ErrorCode_t::NULL_STMT_HANDLE, std::string(ErrorMsg::null_stmt_handle));
    return;
  }
  if(!check_flag(Flag::STMT_PREP)) {
    report_error_status(_DatabaseManager::ErrorCode_t::STMT_UNINITIALIZED, std::string(ErrorMsg::stmt_uninitialized));
    return;
  }
  m_RowDescription = std::make_shared<_DatabaseManager::RowDescription_t>();
  for(std::uint8_t i{0}; i < current_row_size(); ++i) m_RowDescription->insert(i, std::string(sqlite3_column_name(m_Statement.get(), i)));
  set_flag(Flag::ROW_DESCRIPTION_PREP);
}

std::int32_t _DatabaseQuery::prepare_statement_from_file(const std::string& file_path, const std::vector<std::uint32_t>& flags) {
  return prepare_statement_from_str(read_file(file_path), flags);
}

std::int8_t _DatabaseQuery::get_param_count() {
  if(!m_Connection->get_handle()) report_error_status(_DatabaseManager::ErrorCode_t::NULL_DB_HANDLE, std::string(ErrorMsg::null_db_handle));
  else if(!m_Statement) report_error_status(_DatabaseManager::ErrorCode_t::NULL_STMT_HANDLE, std::string(ErrorMsg::null_stmt_handle));
  else if(!m_Connection->is_initialized()) report_error_status(_DatabaseManager::ErrorCode_t::DB_UNINITIALIZED, std::string(ErrorMsg::db_uninitialized));
  else if(!check_flag(Flag::STMT_PREP)) report_error_status(_DatabaseManager::ErrorCode_t::STMT_UNINITIALIZED, std::string(ErrorMsg::stmt_uninitialized));
  else return sqlite3_bind_parameter_count(m_Statement.get());
  return -1;
}

std::string _DatabaseQuery::get_param_name(std::uint8_t index) {
  if (index < 1 || index > get_param_count()) {
    report_error_status(_DatabaseManager::ErrorCode_t::OUT_OF_RANGE_STMT_PARAM_INDEX, std::string(ErrorMsg::out_of_range_stmt_param_look_up));
    return std::string();
  }
  const char* name = sqlite3_bind_parameter_name(m_Statement.get(), index);
  if(!name) return std::string();
  return name;
}

void _DatabaseQuery::bind_param(std::uint8_t index, const _DatabaseManager::SqlParam_t& param) {
  if(index > get_param_count()) {
    report_error_status(_DatabaseManager::ErrorCode_t::OUT_OF_RANGE_STMT_PARAM_INDEX, std::string(ErrorMsg::out_of_range_stmt_param_bind));
    return;
  }
  report_sqlite3_error_status(std::visit(_Sqlite3Binder {index, m_Statement.get()}, param));
}

void _DatabaseQuery::bind_params(const std::vector<_DatabaseManager::SqlParam_t>& params) {
  if(params.size() > static_cast<std::uint64_t>(get_param_count())) report_error_status(_DatabaseManager::ErrorCode_t::TOO_MANY_STMT_PARAMS, std::string(ErrorMsg::too_many_stmt_params));
  for(std::uint8_t i{0}; i < get_param_count(); ++i) bind_param(i+1, params.at(i));
}

void _DatabaseQuery::prepare_cells_descriptions() {
  if(!m_RowDescription) report_error_status(_DatabaseManager::ErrorCode_t::NULL_ROW_DESCRIPTION, std::string(ErrorMsg::null_row_description));
  if(!check_flag(Flag::ROW_DESCRIPTION_PREP)) report_error_status(_DatabaseManager::ErrorCode_t::UNINITIALIZED_ROW_DESCRIPTION, std::string(ErrorMsg::uninitialized_row_description));
  for(_DatabaseManager::RowDescription_t::iterator itr=m_RowDescription->begin(); itr != m_RowDescription->end(); ++itr) {
    m_CellDescriptors.insert({itr->first, std::make_shared<_CellDescriptor>(_CellDescriptor(
      itr->first,
      std::string(itr->second)
    ))});
  }
  set_flag(Flag::CELL_DESCRIPTION_PREP);
}

std::int32_t _DatabaseQuery::exec_from_str(const std::string& query) {
  std::int32_t rc = sqlite3_exec(m_Connection->get_handle(), query.data(), NULL, NULL, NULL);
  report_sqlite3_error_status(rc);
  return rc;
}

std::int32_t _DatabaseQuery::exec_from_file(const std::string& file_path) {
  return exec_from_str(read_file(file_path));
}

std::int32_t _DatabaseQuery::step() {
  std::int32_t rc = sqlite3_step(m_Statement.get());
  report_sqlite3_error_status(rc);
  if(rc != SQLITE_ROW) return rc;
  if(!check_flag(Flag::CELL_DESCRIPTION_PREP)) prepare_cells_descriptions();
  _DatabaseManager::RowData_t row_data;
  for(_DatabaseManager::CellDescriptorContainer_t::iterator itr=m_CellDescriptors.begin(); itr != m_CellDescriptors.end(); ++itr) {
    _DatabaseManager::SqlCell_t data;
    _DatabaseManager::SqlType_t type = static_cast<_DatabaseManager::SqlType_t>(sqlite3_column_type(m_Statement.get(), itr->first));
    switch (type) {
      case _DatabaseManager::SqlType_t::INTEGER: {
        data = sqlite3_column_int64(m_Statement.get(), itr->first);
        break;
      }

      case _DatabaseManager::SqlType_t::DOUBLE: {
        data = sqlite3_column_double(m_Statement.get(), itr->first);
        break;
      }

      case _DatabaseManager::SqlType_t::STRING: {
        const unsigned char* row_text = sqlite3_column_text(m_Statement.get(), itr->first);
        if (row_text) data = std::string(reinterpret_cast<const char*>(row_text), sqlite3_column_bytes(m_Statement.get(), itr->first));
        break;
      }

      case _DatabaseManager::SqlType_t::BINARY_BLOB: {
        const std::uint8_t* blob_head = static_cast<const std::uint8_t*>(sqlite3_column_blob(m_Statement.get(), itr->first));
        data.emplace<std::vector<std::uint8_t>>(blob_head, blob_head+static_cast<std::size_t>(std::max(0, sqlite3_column_bytes(m_Statement.get(), itr->first))));
        break;
      }

      case _DatabaseManager::SqlType_t::NULL_CELL: {
        data = std::monostate();
        break;
      }
    }
    row_data.insert({itr->first, _DatabaseCell(std::move(data), type, itr->second)});
  }
  m_Row = _DatabaseRow(m_RowDescription, std::move(row_data));
  return rc;
}

const _DatabaseRow _DatabaseQuery::get_current_row() { return std::move(m_Row); }

const _DatabaseCell* _DatabaseQuery::get_cell_by_name(std::string name) { return m_Row.get_cell_by_name(name); }
const _DatabaseCell* _DatabaseQuery::get_cell_by_index(std::uint8_t index) { return m_Row.get_cell_by_index(index); }

bool _DatabaseQuery::is_current_row() const { return m_Row.is_row(); }

std::size_t _DatabaseQuery::current_row_size() const { return sqlite3_column_count(m_Statement.get()); }

bool _DatabaseQuery::is_prepared() { return m_Statement.get() && check_flag(Flag::STMT_PREP); }

bool _DatabaseQuery::check_flag(_DatabaseQuery::Flag flag) { return m_StateFlagsArray.test(static_cast<std::uint8_t>(flag)); }
void _DatabaseQuery::set_flag(_DatabaseQuery::Flag flag) { m_StateFlagsArray.set(static_cast<std::uint8_t>(flag), 1); }
void _DatabaseQuery::clear_flag(_DatabaseQuery::Flag flag) { m_StateFlagsArray.set(static_cast<std::uint8_t>(flag), 0); }

_DatabaseManager::Error_t _DatabaseQuery::get_last_error() { return m_Error.back(); }
_DatabaseManager::ErrorCode_t _DatabaseQuery::get_last_error_code() { return m_Error.back().first; }
_DatabaseManager::ErrorMsg_t _DatabaseQuery::get_last_error_msg() { return m_Error.back().second; }
_DatabaseManager::Sqlite3Error_t _DatabaseQuery::get_last_sqlite3_error() { return m_Sqlite3Error.back(); }
_DatabaseManager::Sqlite3ErrorCode_t _DatabaseQuery::get_last_sqlite3_error_code() { return m_Sqlite3Error.back().first; }
_DatabaseManager::Sqlite3ErrorMsg_t _DatabaseQuery::get_last_sqlite3_error_msg() { return m_Sqlite3Error.back().second; }
