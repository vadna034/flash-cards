#include <iostream>
#include <ncurses.h>
#include <sqlite3.h>

#include "db/cardRepo.hpp"
#include "db/collectionRepo.hpp"
#include "db/collection.hpp"
#include "db/internal/db.hpp"

int main() {
  const Db db("flashcards.db");
  db.applyMigrations();

  const CardRepo cardRepo(db);
  const CollectionRepo collectionRepo(db);

  Collection* currentCollection = nullptr;
  std::optional<std::string> currentCollectionId = currentCollection == nullptr ? std::nullopt : std::make_optional(currentCollection->id);
  auto res = collectionRepo.childrenOf(currentCollectionId);

  initscr();
  printw("hello world !!!");
  refresh();
  getch();
  endwin();

  return 0;
}
