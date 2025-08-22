#include "db/cardRepo.hpp"
#include "internal/statement.hpp"

CardRepo::CardRepo(Db &db) : db_(db) {}

std::optional<Card> CardRepo::get(const std::string &id) {
  Statement s(db_.raw(),
              "SELECT id,collection_id,front,back FROM Card WHERE id=?1");
  s.bind(1, id);
  if (s.step())
    return Card{s.col_string(0), s.col_string(1), s.col_string(2),
                s.col_string(3)};
  return std::nullopt;
}

void CardRepo::create(const Card &c) {
  Statement s(
      db_.raw(),
      "INSERT INTO Card(id,collection_id,front,back) VALUES(?1,?2,?3,?4)");
  s.bind(1, c.id);
  s.bind(2, c.collection_id);
  s.bind(3, c.front);
  s.bind(4, c.back);
  s.step();
}

std::vector<Card> CardRepo::listByCollection(const std::string &collectionId) {
  Statement s(
      db_.raw(),
      "SELECT id,collection_id,front,back FROM Card WHERE collection_id=?1");
  s.bind(1, collectionId);
  std::vector<Card> out;
  while (s.step())
    out.push_back(
        {s.col_string(0), s.col_string(1), s.col_string(2), s.col_string(3)});
  return out;
}

void CardRepo::updateFrontBack(const std::string &id, const std::string &front,
                               const std::string &back) {
  Statement s(db_.raw(), "UPDATE Card SET front=?1, back=?2 WHERE id=?3");
  s.bind(1, front);
  s.bind(2, back);
  s.bind(3, id);
  s.step();
}

void CardRepo::remove(const std::string &id) {
  Statement s(db_.raw(), "DELETE FROM Card WHERE id=?1");
  s.bind(1, id);
  s.step();
}
