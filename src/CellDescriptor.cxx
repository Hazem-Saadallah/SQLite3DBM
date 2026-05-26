#include <utility>
#include <CellDescriptor.hxx>

_CellDescriptor::_CellDescriptor(std::uint8_t index, std::string&& name)
: m_Index(index), m_Name(std::move(name)) { }

_CellDescriptor::_CellDescriptor(_CellDescriptor && other) noexcept
: m_Index(std::exchange(other.m_Index, std::uint8_t())),
  m_Name(std::move(other.m_Name)) { }

_CellDescriptor &_CellDescriptor::operator=(_CellDescriptor && other) noexcept {
  if (this == &other) return *this;
  m_Index = std::exchange(other.m_Index, std::uint8_t());
  m_Name = std::move(other.m_Name);
  return *this;
}

const std::uint8_t& _CellDescriptor::get_index() const { return m_Index; }
const std::string& _CellDescriptor::get_name() const { return m_Name; }
