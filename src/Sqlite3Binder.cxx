#include <Sqlite3Binder.hxx>


std::int32_t _Sqlite3Binder::operator()(std::monostate) const { return sqlite3_bind_null(m_Statement, m_Index); }
std::int32_t _Sqlite3Binder::operator()(std::int64_t val) const { return sqlite3_bind_int64(m_Statement, m_Index, val); }
std::int32_t _Sqlite3Binder::operator()(std::double_t val) const { return sqlite3_bind_double(m_Statement, m_Index, val); }
std::int32_t _Sqlite3Binder::operator()(const std::string& val) const { return sqlite3_bind_text(m_Statement, m_Index, val.c_str(), val.size(), SQLITE_TRANSIENT); }
std::int32_t _Sqlite3Binder::operator()(const _DatabaseManager::SqlBlob_t& blob) const { return sqlite3_bind_blob64(m_Statement, m_Index, blob.data(), blob.size(), SQLITE_TRANSIENT); }
