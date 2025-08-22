#pragma once

#include <sqlite3.h>
#include <stdexcept>

// ---------- Errors ----------
struct DbError : std::runtime_error {
  using std::runtime_error::runtime_error;
};
struct NotFoundError : DbError {
  using DbError::DbError;
};
struct ConstraintViolationError : DbError {
  using DbError::DbError;
};

/*
 * Throws an error if SQLite returns an invalid return code
 */
inline void throwOnSqlite(int rc, sqlite3 *db, const char *context) {
  if (rc == SQLITE_OK || rc == SQLITE_ROW || rc == SQLITE_DONE)
    return;
  const char *msg = sqlite3_errmsg(db);
  if (rc == SQLITE_CONSTRAINT)
    throw ConstraintViolationError(std::string(context) + ": " + msg);
  throw DbError(std::string(context) + ": " + msg);
}

/*
 * Wrapper to handle database initialization or throw an exception
 */
inline void throwOnExec(sqlite3 *db, const std::string &sql) {
  char *err = nullptr;
  int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err);
  if (rc != SQLITE_OK) {
    std::string msg = err ? err : sqlite3_errmsg(db);
    if (err)
      sqlite3_free(err);
    throw DbError("exec failed: " + msg);
  }
}

/* Wrapper for sqlite3_open_v2; closes db on failure and throws DbError. */
inline void throwOnDatabaseOpen(sqlite3 *db, int rc, const std::string &path) {
  if (rc == SQLITE_OK)
    return;
  const char *msg = db ? sqlite3_errmsg(db) : "sqlite3_open_v2 failed";
  if (db)
    sqlite3_close(db);
  throw DbError("open " + path + ": " + std::string(msg));
}
