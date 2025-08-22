#include "db.hpp"
#include "errors.hpp"

Db::Db(const std::string &path) {
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX;

  int rc = sqlite3_open_v2(path.c_str(), &db_, flags, nullptr);
  throwOnDatabaseOpen(db_, rc, path);

  exec("PRAGMA foreign_keys = ON;");
  exec("PRAGMA journal_mode = WAL;");
  exec("PRAGMA synchronous = NORMAL;");
}

Db::~Db() {
  if (db_ != nullptr) {
    sqlite3_close(db_);
  }
}

void Db::exec(const std::string &sql) {
    throwOnExec(db_, sql);
}

sqlite3* Db::raw() const { return db_; }
