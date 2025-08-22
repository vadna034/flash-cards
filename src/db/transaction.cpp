#include "transaction.hpp"
#include "internal/db.hpp"

Transaction::Transaction(Db &db) : db_(db) {
  db_.exec("BEGIN");
  active_ = true;
}

Transaction::~Transaction() {
  if (active_)
    try {
      db_.exec("ROLLBACK");
    } catch (...) {
    }
}

void Transaction::commit() {
  db_.exec("COMMIT");
  active_ = false;
}

