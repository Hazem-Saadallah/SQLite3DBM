#pragma once

#include <string_view>

namespace ErrorMsg {
  constexpr std::string_view file_not_found = "file '{}' was not found";
  constexpr std::string_view file_not_open = "file '{}' could not be opened";
  constexpr std::string_view file_read_success = "file '{}' was read successfully";
  constexpr std::string_view sqlite3_internal_error = "Check SQLite3 internal error using the provided interface";
  constexpr std::string_view null_db_handle = "Database handle is null or nullptr";
  constexpr std::string_view null_stmt_handle = "Statement is null or nullptr";
  constexpr std::string_view db_uninitialized = "Database is uninitialized";
  constexpr std::string_view stmt_uninitialized = "Statement is uninitialized";
  constexpr std::string_view out_of_range_stmt_param_bind = "The binding index is bigger than the number of parameters in this statement";
  constexpr std::string_view out_of_range_stmt_param_look_up = "The looked up index is bigger than the number of parameters in this statement";
  constexpr std::string_view too_many_stmt_params = "The number of parameters available in the vector are more than the statement receives. Only the needed parameters are used";
  constexpr std::string_view null_row_description = "The Row Descriptor is Null or Empty";
  constexpr std::string_view uninitialized_row_description = "The Row Descriptor Has Not Been Initialized";
}
