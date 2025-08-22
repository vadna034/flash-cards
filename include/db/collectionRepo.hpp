#pragma once

#include <optional>
#include <string>
#include <vector>

#include "../../src/db/internal/db.hpp"
#include "collection.hpp"

/**
 * Repository for CRUD and hierarchy operations on the Collection table.
 *
 * Notes:
 *  - All methods may throw DBError (or a derived type) when SQLite reports
 *    an error (constraint violation, I/O error, database locked, etc.).
 *  - `childrenOf` returns only **direct** children (one level), not a recursive
 *    descendant list.
 *  - `move` allows setting a collection's parent to null (root). It does not
 *    detect hierarchy cycles unless you enforce them in the schema (e.g., via
 *    triggers/closure table).
 */
class CollectionRepo {
  Db &db_;

public:
  /**
   * Construct a repository bound to a Db connection.
   */
  explicit CollectionRepo(Db &db);

  /**
   * Retrieve a collection by id.
   *
   * @param id  Collection id to look up.
   * @return    Collection if found; std::nullopt otherwise.
   *
   * @throws DBError on SQLite execution errors.
   */
  std::optional<Collection> get(const std::string &id); 

  /**
   * Insert a new collection.
   *
   * @param c  Collection to insert. Caller should ensure:
   *           - c.node_type == "collection" (if you store node_type here)
   *           - c.parent_id, if present, references an existing Collection
   *
   * @throws DBError if insertion fails (e.g., UNIQUE/FK/check constraint, OOM).
   */
  void create(const Collection &c); 

  /**
   * List direct child collections under a given parent.
   *
   * @param id  Parent collection id.
   * @return    Vector of Collections whose parent is `id`.
   *
   * @throws DBError if query execution fails.
   */
  std::vector<Collection> childrenOf(const std::string &id); 

  /**
   * Move a collection to a new parent (or root).
   *
   * @param id         Collection to reparent.
   * @param newParent  New parent id, or std::nullopt for root.
   *
   * @throws DBError if update fails (e.g., FK violation, check constraint).
   * @warning This does not guard against creating cycles unless your schema
   *          enforces it (e.g., via triggers/closure table). Consider adding
   *          application-level checks if needed.
   */
  void move(const std::string &id, const std::optional<std::string> &newParent); 

  /**
   * Delete a collection by id.
   *
   * Behavior depends on your FK constraints:
   *  - If children/cards exist and ON DELETE RESTRICT is in effect, this will
   *    throw due to FK violation.
   *  - If you use ON DELETE CASCADE (not typical for collections), children
   *    may be deleted automatically.
   *
   * @param id  Collection id to delete.
   *
   * @throws DBError if deletion fails (FK violation, I/O error, etc.).
   */
  void remove(const std::string &id); 
};
