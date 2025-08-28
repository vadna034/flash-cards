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

void Db::exec(const std::string &sql) const {
    throwOnExec(db_, sql);
}

sqlite3* Db::raw() const { return db_; }

void Db::applyMigrations() const {
    exec("CREATE TABLE IF NOT EXISTS Collection ("
            "id INTEGER PRIMARY KEY, "
            "name TEXT NOT NULL, "
            "node_type TEXT NOT NULL CHECK (node_type IN ('collection','card')), "
            "parent_collection TEXT REFERENCES Collection(id))");

    exec("CREATE TABLE IF NOT EXISTS Card ("
            "id INTEGER PRIMARY KEY, "
            "collection_id INTEGER NOT NULL REFERENCES Collection(id), "
            "front TEXT NOT NULL, "
            "back TEXT NOT NULL)");
}

