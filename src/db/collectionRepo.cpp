#include "db/collectionRepo.hpp"
#include "internal/db.hpp"
#include "internal/statement.hpp"
#include <optional>
#include <sqlite3.h>

CollectionRepo::CollectionRepo(Db &db) : db_(db) {}

std::optional<Collection> CollectionRepo::get(int64_t collectionId) const{
  Statement s(db_.raw(), "SELECT id,name,node_type,parent_collection FROM "
                         "Collection WHERE id=?1");
  s.bind(1, collectionId);
  if (s.step()) {
    Collection c{s.col_int64(0), s.col_string(1), s.col_string(2), std::nullopt};
    int64_t p = s.col_int64(3);
    if (sqlite3_column_type(s.raw(), 3) != SQLITE_NULL) c.parent_id = std::make_optional(p);
    return c;
  }
  return std::nullopt;
}

void CollectionRepo::create(const Collection &c) const{
  Statement s(db_.raw(),
              "INSERT INTO Collection(id,name,node_type,parent_collection) "
              "VALUES(?1,?2,?3,?4)");
  s.bind(1, c.id);
  s.bind(2, c.name);
  s.bind(3, c.node_type);
  if (c.parent_id) {
    s.bind(4, *c.parent_id);
  } else {
    s.bind(4, nullptr);
  }
  s.step();
}

std::vector<Collection> CollectionRepo::childrenOf(int64_t parentId) const {
    Statement s(db_.raw(),
        "SELECT id, name, node_type, parent_collection "
        "FROM Collection WHERE parent_collection = ?1 ORDER BY name");
    s.bind(1, parentId);

    std::vector<Collection> out;
    while (s.step()) {
        Collection c;
        c.id = s.col_int64(0);
        c.name = s.col_string(1);
        c.node_type = s.col_string(2);
        // parent may be NULL; check column type before reading
        if (sqlite3_column_type(s.raw(), 3) != SQLITE_NULL)
            c.parent_id = s.col_int64(3);
        else
            c.parent_id.reset();
        out.push_back(std::move(c));
    }
    return out;
}

void CollectionRepo::move(int64_t id,
                          const std::optional<std::string> &newParent) const{
  Statement s(db_.raw(),
              "UPDATE Collection SET parent_collection=?1 WHERE id=?2");
  if (newParent) {
    s.bind(1, *newParent);
  } else {
    s.bind(1, nullptr);
  }

  s.bind(2, id);
  s.step();
}

void CollectionRepo::remove(int64_t id) const{
  Statement s(db_.raw(), "DELETE FROM Collection WHERE id=?1");
  s.bind(1, id);
  s.step();
}
