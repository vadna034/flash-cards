#pragma once

#include <sqlite3.h>
#include <string>

/**
 * RAII wrapper for an SQLite database connection.
 *
 * Responsibilities:
 *  - Open and close the database safely.
 *  - Apply recommended PRAGMAs for foreign key enforcement and journaling.
 *  - Provide a minimal API (`exec`, `raw`) for internal use.
 *
 * Users of the higher-level API (repos/Tx) should not normally touch this class directly.
 */
class Db {
  sqlite3 *db_{nullptr};

public:
  /**
   * Open a connection to the SQLite database at `path`.
   *
   * Applies the following PRAGMAs automatically:
   *  - `foreign_keys = ON` (enforce FK constraints)
   *  - `journal_mode = WAL` (improves concurrency & durability)
   *  - `synchronous = NORMAL` (good performance/durability tradeoff)
   *
   * @param path   Path to the SQLite database file. Use `:memory:` for in-memory DB.
   *
   * @throws DBError if the database cannot be opened (invalid path, permissions, OOM).
   */
  explicit Db(const std::string &path);

  /**
   * Destructor that closes the SQLite connection and frees memory.
   * Never throws.
   */
  ~Db();

  /**
   * Execute a single SQL string (no parameter binding).
   *
   * Typical use cases:
   *  - DDL (CREATE TABLE, CREATE INDEX)
   *  - PRAGMAs
   *  - Migrations
   *
   * Not recommended for DML (INSERT/UPDATE/DELETE); use prepared statements instead.
   *
   * @param sql   SQL statement string.
   *
   * @throws DBError if execution fails (syntax error, constraint violation, disk I/O, etc.).
   */
  void exec(const std::string &sql) const;

  /**
   * Accessor for the raw sqlite3* handle.
   *
   * This is primarily for internal use (e.g., to pass to Statement).
   *
   * @return sqlite3* pointer owned by this Db instance.
   */
  sqlite3 *raw() const;

  void applyMigrations() const;
};
