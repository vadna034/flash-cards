#pragma once

#include "sqlite3.h"
#include <string>

/**
 * RAII wrapper around sqlite3 prepared statements.
 * 
 * Notes:
 *  - Bind parameter indices are **1-based**, matching SQLite's API.
 *  - For non-SELECT statements (INSERT/UPDATE/DELETE), call step() **once**.
 *  - For SELECT queries, iterate rows with `while (step()) { ... }` until it returns false.
 */
class Statement {
  sqlite3_stmt *statement_{nullptr};
  sqlite3 *db_{nullptr};

  /** Returns raw UTF-8 text pointer for column i (may be nullptr). Never throws. */
  const unsigned char *col_text(int i) const; 

public:
  /**
   * Construct and prepare a new statement.
   *
   * @param db                Open sqlite3 handle.
   * @param statementString   SQL text (may contain ?1, ?2... placeholders).
   *
   * @throws DBError on prepare failure (e.g., SQL syntax error, OOM).
   */
  Statement(sqlite3 *db, const std::string &statementString); 

  /**
   * Finalizes the prepared statement (sqlite3_finalize). Never throws.
   */
  ~Statement(); 

  /**
   * Bind a UTF-8 text value to 1-based parameter index `idx`.
   *
   * @throws DBError if binding fails (e.g., invalid index, OOM).
   */
  void bind(int idx, const std::string &v) const; 

  /**
   * Bind a SQL NULL to 1-based parameter index `idx`.
   *
   * @throws DBError if binding fails (e.g., invalid index).
   */
  void bind(int idx, std::nullptr_t) const; 

  /**
   * Advance the statement execution.
   *
   * For SELECT:
   *   - Returns true when a row is available (SQLITE_ROW).
   *   - Returns false when there are no more rows (SQLITE_DONE).
   *
   * For INSERT/UPDATE/DELETE:
   *   - Executes the statement and returns false (SQLITE_DONE).
   *   - Call exactly once per execution.
   *
   * @return bool  True if a row is available, false if completed.
   * @throws DBError if SQLite reports an error (e.g., constraint violation,
   *                 misuse, disk I/O, database locked).
   */
  bool step() const; 

  /**
   * Reset the statement to its initial state, ready to be rebound and executed again.
   * Also clears all previous bindings.
   *
   * @throws DBError if reset/clear_bindings fails (rare; e.g., OOM).
   */
  void reset() const; 

  /**
   * Get column `i` as a std::string.
   *
   * Behavior:
   *  - Returns an empty string if the column is NULL.
   *  - Copies the text (so it remains valid after the next step/reset/finalize).
   *
   * @throws no exceptions.
   */
  std::string col_string(int i) const; 
};
