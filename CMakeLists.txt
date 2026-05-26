cmake_minimum_required(VERSION 3.31)

project(SQLite3DBM LANGUAGES C CXX VERSION 0.1.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_BUILD_TOOL make)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)

FetchContent_Declare(
  sqlite3 
  URL https://sqlite.org/2025/sqlite-amalgamation-3510100.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/sqlite3
  BINARY_DIR ${CMAKE_BINARY_DIR}/extern/sqlite3
)
FetchContent_MakeAvailable(sqlite3)

add_library(sqlite3 SHARED ${sqlite3_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
if(UNIX)
  target_link_libraries(sqlite3 PUBLIC ${CMAKE_DL_LIBS} pthread)
endif()

# -----------------------------------------------------------------------------
set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SQLite3DBM/DatabaseManager.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SQLite3DBM/DatabaseConnection.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SQLite3DBM/CellDescriptor.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SQLite3DBM/DatabaseRow.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SQLite3DBM/DatabaseCell.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SQLite3DBM/DatabaseQuery.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SQLite3DBM/Sqlite3Binder.cxx
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC sqlite3)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

set(BIN_NAME bin.out)
set(TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cxx
  # ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cxx
  # ${CMAKE_CURRENT_SOURCE_DIR}/tests/performance.cxx
)

add_executable(${BIN_NAME} ${TEST_SOURCES})
target_include_directories(${BIN_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${BIN_NAME} PRIVATE ${PROJECT_NAME})
target_compile_options(${BIN_NAME} PRIVATE -Wall -Wextra)
