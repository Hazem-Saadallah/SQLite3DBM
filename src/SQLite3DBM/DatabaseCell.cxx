#include <memory>
#include <optional>
#include <variant>
#include <SQLite3DBM/DatabaseCell.hxx>

_DatabaseCell::_DatabaseCell(_DatabaseManager::SqlCell_t&& data, const _DatabaseManager::SqlType_t& type, const std::shared_ptr<_CellDescriptor>& cell_descriptor)
: m_Data(std::move(data)), m_Type(type), m_CellDescription(std::move(cell_descriptor)) {}

_DatabaseCell::_DatabaseCell(_DatabaseCell && other) noexcept
: m_Data(std::move(other.m_Data)), m_Type(other.m_Type), m_CellDescription(std::move(other.m_CellDescription)) {}

_DatabaseCell &_DatabaseCell::operator=(_DatabaseCell && other) noexcept {
  if(this == &other) return *this;
  m_Data = std::move(other.m_Data);
  m_Type = other.m_Type;
  other.m_Type = _DatabaseManager::SqlType_t();
  m_CellDescription = std::move(other.m_CellDescription);
  return *this;
}

std::shared_ptr<_CellDescriptor> _DatabaseCell::get_cell_descriptor() const { return m_CellDescription; }
std::string _DatabaseCell::get_name() const { return m_CellDescription->get_name(); }
_DatabaseManager::SqlType_t _DatabaseCell::get_type() const { return m_Type; }
std::uint8_t _DatabaseCell::get_index() const { return m_CellDescription->get_index(); }

const std::optional<std::int64_t> _DatabaseCell::as_integer() const {
  if(const std::int64_t* ptr = std::get_if<std::int64_t>(&m_Data)) return *ptr;
  return std::nullopt;
}

const std::optional<std::double_t> _DatabaseCell::as_double() const {
  if(const std::double_t* ptr = std::get_if<std::double_t>(&m_Data)) return *ptr;
  return std::nullopt;
}

const std::optional<std::string> _DatabaseCell::as_string() const {
  if(const std::string* ptr = std::get_if<std::string>(&m_Data)) return *ptr;
  return std::nullopt;
}

const std::optional<_DatabaseManager::SqlBlob_t> _DatabaseCell::as_blob() const {
  if(const _DatabaseManager::SqlBlob_t* ptr = std::get_if<_DatabaseManager::SqlBlob_t>(&m_Data)) return *ptr;
  return std::nullopt;
}

bool _DatabaseCell::is_null() const { return get_type() == _DatabaseManager::SqlType_t::NULL_CELL; }
bool _DatabaseCell::is_integer() const { return get_type() == _DatabaseManager::SqlType_t::INTEGER; }
bool _DatabaseCell::is_double() const { return get_type() == _DatabaseManager::SqlType_t::DOUBLE; }
bool _DatabaseCell::is_string() const {return get_type() == _DatabaseManager::SqlType_t::STRING; };
bool _DatabaseCell::is_blob() const { return get_type() == _DatabaseManager::SqlType_t::BINARY_BLOB; }
