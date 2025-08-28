#include "db/collectionRepo.hpp"
#include "internal/db.hpp"
#include "internal/statement.hpp"

CollectionRepo::CollectionRepo(const Db &db) : db_(db) {}

std::optional<Collection> CollectionRepo::get(const std::string &id) const {
  Statement s(db_.raw(), "SELECT id,name,node_type,parent_collection FROM "
                         "Collection WHERE id=?1");
  s.bind(1, id);
  if (s.step()) {
    Collection c{s.col_string(0), s.col_string(1), s.col_string(2), {}};
    std::string parent = s.col_string(3);
    if (!parent.empty())
      c.parent_id = parent;
    return c;
  }
  return std::nullopt;
}

void CollectionRepo::create(const Collection &c) const {
  Statement s(db_.raw(),
              "INSERT INTO Collection(id,name,node_type,parent_collection) "
              "VALUES(?1,?2,?3,?4)");
  s.bind(1, nullptr); // pass null to take advantage of auto incrementing id
  s.bind(2, c.name);
  s.bind(3, c.node_type);
  if (c.parent_id) {
    s.bind(4, *c.parent_id);
  } else {
    s.bind(4, nullptr);
  }
  s.step();
}

std::vector<Collection>
CollectionRepo::childrenOf(const std::optional<std::string> &id) const {
  std::string statementString;

  if (id) {
    statementString = "SELECT id,name,node_type,parent_collection FROM Collection WHERE parent_collection=?1";
  } else {
    statementString = "SELECT id,name,node_type,parent_collection FROM Collection WHERE parent_collection IS NULL";
  }

  Statement s(db_.raw(), statementString);


  if (id) {
    s.bind(1, *id);
  }

  std::vector<Collection> out;

  while (s.step()) {
    Collection c{s.col_string(0), s.col_string(1), s.col_string(2), {}};
    std::string parent = s.col_string(3);
    if (!parent.empty()) {
      c.parent_id = parent;
    }
    out.push_back(std::move(c));
  }
  return out;
}

void CollectionRepo::move(const std::string &id,
                          const std::optional<std::string> &newParent) const {
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

void CollectionRepo::remove(const std::string &id) const {
  Statement s(db_.raw(), "DELETE FROM Collection WHERE id=?1");
  s.bind(1, id);
  s.step();
}
