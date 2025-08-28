#include <iostream>
#include <sqlite3.h>

#include "db/cardRepo.hpp"
#include "db/collectionRepo.hpp"
#include "db/internal/db.hpp"

int main(){
    const Db db("flashcards.db");
    db.applyMigrations();

    const CardRepo cardRepo(db);
    const CollectionRepo collectionRepo(db);
    auto res = collectionRepo.childrenOf(std::nullopt);



    return 0;
}
