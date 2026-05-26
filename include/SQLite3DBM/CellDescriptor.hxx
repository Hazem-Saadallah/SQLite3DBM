#pragma once

#include <string>
#include <cstdint>
#include <SQLite3DBM/BiMap.hxx>
#include <SQLite3DBM/DatabaseManager.hxx>

class _CellDescriptor {
private:
  std::uint8_t m_Index = std::uint8_t();
  std::string m_Name = std::string();

public:
  _CellDescriptor(std::uint8_t index, std::string&& name);
  ~_CellDescriptor() = default;

  _CellDescriptor(_CellDescriptor && other) noexcept;
  _CellDescriptor &operator=(_CellDescriptor && other) noexcept;

  _CellDescriptor(const _CellDescriptor & other) = delete;
  _CellDescriptor &operator=(const _CellDescriptor & other) = delete; 

  const std::uint8_t& get_index() const;
  const std::string& get_name() const;
};
