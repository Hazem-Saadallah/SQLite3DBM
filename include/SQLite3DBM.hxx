#include <memory>
#include <DatabaseRow.hxx>
#include <DatabaseQuery.hxx>
#include <DatabaseManager.hxx>
#include <DatabaseConnection.hxx>

namespace SQLite3DBM {
  using namespace _DatabaseManager;
  using Connection = _DatabaseConnection;
  typedef std::shared_ptr<_DatabaseConnection> ConnectionPtr;
  using Query = _DatabaseQuery;
  using Row = _DatabaseRow;
}
