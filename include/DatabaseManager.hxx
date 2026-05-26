#pragma once

#include <cmath>
#include <string>
#include <cstdint>
#include <sqlite3.h>
#include <utility>
#include <variant>
#include <vector>
#include <BiMap.hxx>

namespace _DatabaseManager {
  enum class ErrorCode_t : std::uint8_t {
    OK,
    QUERY_FILE_NOT_FOUND,
    QUERY_FILE_FAILED_OPEN,
    NULL_DB_HANDLE,
    NULL_STMT_HANDLE,
    DB_UNINITIALIZED,
    STMT_UNINITIALIZED,
    OUT_OF_RANGE_STMT_PARAM_INDEX,
    TOO_MANY_STMT_PARAMS,
    SQLITE3_CODE,
    NULL_ROW_DESCRIPTION,
    UNINITIALIZED_ROW_DESCRIPTION,
  };
  enum class SqlType_t : std::uint8_t {
    INTEGER = SQLITE_INTEGER,
    DOUBLE = SQLITE_FLOAT,
    BINARY_BLOB = SQLITE_BLOB,
    STRING = SQLITE_TEXT,
    NULL_CELL = SQLITE_NULL,
  };

  typedef std::string ErrorMsg_t;
  typedef std::pair<ErrorCode_t, ErrorMsg_t> Error_t;

  typedef std::int32_t Sqlite3ErrorCode_t;
  typedef std::string Sqlite3ErrorMsg_t;
  typedef std::pair<Sqlite3ErrorCode_t, Sqlite3ErrorMsg_t> Sqlite3Error_t;
  typedef std::vector<std::uint8_t> SqlBlob_t;
  typedef std::variant<std::monostate, std::int64_t, std::double_t, std::string, SqlBlob_t> SqlParam_t;
  typedef SqlParam_t SqlCell_t;

  namespace BLOB_IO {
    SqlBlob_t read_blob(std::string file_path);
    void write_blob(std::string file_path, SqlBlob_t data);
  }
};
