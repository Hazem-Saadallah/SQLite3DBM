#include <memory>
#include <SQLite3DBM/DatabaseRow.hxx>
#include <SQLite3DBM/DatabaseQuery.hxx>
#include <SQLite3DBM/DatabaseManager.hxx>
#include <SQLite3DBM/DatabaseConnection.hxx>

namespace SQLite3DBM {
  using namespace _DatabaseManager;
  using Connection = _DatabaseConnection;
  typedef std::shared_ptr<_DatabaseConnection> ConnectionPtr;
  using Query = _DatabaseQuery;
  using Row = _DatabaseRow;
}
