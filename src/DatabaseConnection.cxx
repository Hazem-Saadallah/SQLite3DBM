#include <memory>
#include <string>
#include <DatabaseManager.hxx>
#include <ErrorMsg.hxx>
#include <Sqlite3Binder.hxx>
#include <DatabaseConnection.hxx>
#include <utility>


_DatabaseConnection::_DatabaseConnection(const std::string& db_name) {
  std::int32_t rc = sqlite3_open(db_name.c_str(), &m_Handle);
  report_sqlite3_error_status(rc);
  if (rc == SQLITE_OK) set_flag(Flag::DATABASE_INIT);
}

_DatabaseConnection::_DatabaseConnection(_DatabaseConnection && other) noexcept
: m_Handle(other.m_Handle),
  m_StateFlagsArray(other.m_StateFlagsArray),
  m_Error(std::move(other.m_Error)),
  m_Sqlite3Error(other.m_Sqlite3Error) {
  other.m_StateFlagsArray.reset();
  other.m_Handle = nullptr;
}

_DatabaseConnection &_DatabaseConnection::operator=(_DatabaseConnection && other) noexcept {
  if(this == &other) return *this;
  close_database();
  m_Handle = other.m_Handle;
  other.m_Handle = nullptr;
  m_StateFlagsArray = std::move(other.m_StateFlagsArray);
  other.m_StateFlagsArray.reset();
  m_Error = std::move(other.m_Error);
  m_Sqlite3Error = std::move(other.m_Sqlite3Error);
  return *this;
}

_DatabaseConnection::~_DatabaseConnection() {
  close_database();
}

bool _DatabaseConnection::is_initialized() {
  return m_Handle && check_flag(Flag::DATABASE_INIT);
}

std::int64_t _DatabaseConnection::get_last_insert_rowid() {
  return sqlite3_last_insert_rowid(m_Handle);
}

std::int32_t _DatabaseConnection::close_database() {
  std::int32_t close_rc = SQLITE_OK;
  if (m_Handle) {
    close_rc = sqlite3_close(m_Handle);
    if (close_rc == SQLITE_OK) m_Handle = nullptr;
  }
  return close_rc;
}

void _DatabaseConnection::report_error_status(_DatabaseManager::ErrorCode_t error_code, const _DatabaseManager::ErrorMsg_t& error_msg) {
  m_Error.push_back(_DatabaseManager::Error_t(error_code, error_msg));
}

void _DatabaseConnection::report_sqlite3_error_status(_DatabaseManager::Sqlite3ErrorCode_t error_code) {
  report_error_status(_DatabaseManager::ErrorCode_t::SQLITE3_CODE, std::string(ErrorMsg::sqlite3_internal_error));
  m_Sqlite3Error.push_back(_DatabaseManager::Sqlite3Error_t(error_code, std::string(sqlite3_errmsg(m_Handle))));
}

void _DatabaseConnection::report_external_sqlite3_error_status(_DatabaseManager::Sqlite3ErrorCode_t error_code) {
  m_Sqlite3Error.push_back(_DatabaseManager::Sqlite3Error_t(error_code, std::string(sqlite3_errmsg(m_Handle))));
}

sqlite3* _DatabaseConnection::get_handle() { return m_Handle; }

bool _DatabaseConnection::check_flag(_DatabaseConnection::Flag flag) { return m_StateFlagsArray.test(static_cast<std::uint8_t>(flag)); }
void _DatabaseConnection::set_flag(_DatabaseConnection::Flag flag) { m_StateFlagsArray.set(static_cast<std::uint8_t>(flag), 1); }
void _DatabaseConnection::clear_flag(_DatabaseConnection::Flag flag) { m_StateFlagsArray.set(static_cast<std::uint8_t>(flag), 0); }

_DatabaseManager::Error_t _DatabaseConnection::get_last_error() { return m_Error.back(); }
_DatabaseManager::ErrorCode_t _DatabaseConnection::get_last_error_code() { return m_Error.back().first; }
_DatabaseManager::ErrorMsg_t _DatabaseConnection::get_last_error_msg() { return m_Error.back().second; }
_DatabaseManager::Sqlite3Error_t _DatabaseConnection::get_last_sqlite3_error() { return m_Sqlite3Error.back(); }
_DatabaseManager::Sqlite3ErrorCode_t _DatabaseConnection::get_last_sqlite3_error_code() { return m_Sqlite3Error.back().first; }
_DatabaseManager::Sqlite3ErrorMsg_t _DatabaseConnection::get_last_sqlite3_error_msg() { return m_Sqlite3Error.back().second; }

std::shared_ptr<_DatabaseConnection> _DatabaseConnection::Create(const std::string& db_name) { return std::make_shared<_DatabaseConnection>(db_name); }
