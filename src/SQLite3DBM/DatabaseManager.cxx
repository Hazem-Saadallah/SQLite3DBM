#include <format>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <SQLite3DBM/DatabaseManager.hxx>

_DatabaseManager::SqlBlob_t _DatabaseManager::BLOB_IO::read_blob(std::string file_path) {
  std::ifstream file(file_path, std::ios::binary | std::ios::ate);
  if(!std::filesystem::exists(file_path)) throw std::runtime_error(std::format("File '{}' Does Not exist", file_path));
  if(!file.is_open()) throw std::runtime_error(std::format("Could Not Open File '{}'", file_path));

  std::streamsize ssize = file.tellg();
  if(ssize <= 0) return SqlBlob_t();

  file.seekg(0, std::ios::beg);

  std::vector<std::uint8_t> buffer(ssize);
  if(file.read(reinterpret_cast<char*>(buffer.data()),ssize)) return buffer;
  return SqlBlob_t();
}

void _DatabaseManager::BLOB_IO::write_blob(std::string file_path, SqlBlob_t data) {
  std::ofstream file(file_path, std::ios::binary);
  if(!file.is_open()) throw std::runtime_error(std::format("Could Not Open File '{}'", file_path));
  file.write(reinterpret_cast<const char*>(data.data()), data.size());
}
