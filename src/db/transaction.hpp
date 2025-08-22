#pragma once

#include "internal/db.hpp"

/**
 * RAII wrapper for a SQLite transaction.
 *
 * Usage pattern:
 *   {
 *     Transaction tx(db);
 *     // ... perform repo operations here ...
 *     tx.commit();  // must be called explicitly to persist changes
 *   } // if commit() not called, destructor rolls back
 *
 * Semantics:
 *  - Begins a transaction on construction (`BEGIN`).
 *  - If `commit()` is not called before destruction, issues a `ROLLBACK`.
 *  - Ensures database consistency even if an exception escapes the scope.
 */
class Transaction {
  Db &db_;
  bool active_{false};

public:
  /**
   * Begin a new transaction (`BEGIN`).
   *
   * @param db  Open Db connection to use.
   *
   * @throws DBError if starting the transaction fails
   *         (e.g., locked database, I/O error).
   */
  explicit Transaction(Db &db);

  /**
   * Destructor: rolls back if still active (not committed).
   *
   * Never throws. If rollback itself fails, the error is swallowed
   * because destructors must not throw.
   */
  ~Transaction();

  /**
   * Commit the transaction (`COMMIT`).
   *
   * After calling this, the transaction is no longer active.
   *
   * @throws DBError if commit fails (I/O error, locked database, etc.).
   */
  void commit();
};

