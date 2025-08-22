#pragma once

#include <string>
#include <vector>
#include <optional>
#include "card.hpp"
#include "internal/db.hpp"

/**
 * Repository for performing CRUD operations on the Card table.
 *
 * All operations throw DBError (or subclasses, if you define them) when SQLite
 * reports a failure (constraint violation, I/O error, misuse, etc.).
 */
class CardRepo {
  Db &db_;

public:
  /**
   * Construct repository bound to a Db connection.
   */
  explicit CardRepo(Db &db);

  /**
   * Retrieve a card by id.
   *
   * @param id   Card id to look up.
   * @return     Card object if found, std::nullopt otherwise.
   *
   * @throws DBError if SQLite execution fails (I/O, locked database, etc.).
   */
  std::optional<Card> get(const std::string &id);

  /**
   * Insert a new card.
   *
   * @param c   Card to insert.
   *
   * @throws DBError if insertion fails (constraint violation, invalid FK, OOM, etc.).
   */
  void create(const Card &c);

  /**
   * List all cards belonging to a given collection.
   *
   * @param collectionId   Parent collection id.
   * @return               Vector of cards belonging to that collection.
   *
   * @throws DBError if query execution fails.
   */
  std::vector<Card> listByCollection(const std::string &collectionId);

  /**
   * Update both front and back text of an existing card.
   *
   * @param id     Card id to update.
   * @param front  New front text.
   * @param back   New back text.
   *
   * @throws DBError if update fails (constraint violation, missing row, I/O).
   */
  void updateFrontBack(const std::string &id, const std::string &front, const std::string &back);

  /**
   * Delete a card by id.
   *
   * @param id   Card id to delete.
   *
   * @throws DBError if deletion fails (I/O, locked database, etc.).
   */
  void remove(const std::string &id);
};

