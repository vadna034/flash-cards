#include "statement.hpp"
#include "errors.hpp"

Statement::Statement(sqlite3 *db, const std::string &statementString)
    : db_(db) {
  int rc =
      sqlite3_prepare_v2(db, statementString.c_str(), -1, &statement_, nullptr);
  throwOnSqlite(rc, db, ("prepare: " + statementString).c_str());
}

Statement::~Statement() {
  if (statement_)
    sqlite3_finalize(statement_);
}

void Statement::bind(int idx, const std::string &v) const {
  throwOnSqlite(
      sqlite3_bind_text(statement_, idx, v.c_str(), -1, SQLITE_TRANSIENT), db_,
      "bind_text");
}

void Statement::bind(int idx, std::nullptr_t) const {
  throwOnSqlite(sqlite3_bind_null(statement_, idx), db_, "bind_null");
}

bool Statement::step() const{
  int rc = sqlite3_step(statement_);
  if (rc == SQLITE_ROW) {
    return true;
  }
  if (rc == SQLITE_DONE) {
    return false;
  }
  throwOnSqlite(rc, db_, "step");
  return false;
}

void Statement::reset() const {
  throwOnSqlite(sqlite3_reset(statement_), db_, "reset");
  throwOnSqlite(sqlite3_clear_bindings(statement_), db_, "clear_bindings");
}

const unsigned char *Statement::col_text(int i) const {
  return sqlite3_column_text(statement_, i);
}
std::string Statement::col_string(int i) const {
  const unsigned char *t = col_text(i);
  return t ? std::string(reinterpret_cast<const char *>(t)) : std::string();
}
