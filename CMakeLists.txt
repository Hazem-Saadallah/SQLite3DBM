include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.31)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_BUILD_TOOL make)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(BIN_NAME bin.out)
set(COMMON_COMPILE_OPTIONS -Wall -Wextra)
set(INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/sqlite3
)
set(COMMON_LIBS
  sqlite3
)

set(SOURCES
  # ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cxx
  # ${CMAKE_CURRENT_SOURCE_DIR}/tests/performance.cxx

  ${CMAKE_CURRENT_SOURCE_DIR}/src/DatabaseManager.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DatabaseConnection.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CellDescriptor.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DatabaseRow.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DatabaseCell.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CellDescriptor.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DatabaseCell.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DatabaseQuery.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Sqlite3Binder.cxx
)

FetchContent_Declare(
  sqlite3 
  URL https://sqlite.org/2025/sqlite-amalgamation-3510100.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/sqlite3
  BINARY_DIR ${CMAKE_BINARY_DIR}/extern/sqlite3
)

FetchContent_MakeAvailable(sqlite3)
add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
if(UNIX)
  target_link_libraries(sqlite3 PUBLIC ${CMAKE_DL_LIBS} pthread)
endif()

include_directories(${INCLUDE_DIR})

project(CUT LANGUAGES C CXX VERSION 0.1.2)
add_executable(${BIN_NAME} ${SOURCES})
target_link_libraries(${BIN_NAME} PRIVATE ${COMMON_LIBS})
target_compile_options(${BIN_NAME} PRIVATE ${COMMON_COMPILE_OPTIONS})
