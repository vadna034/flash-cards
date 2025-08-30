#include <iostream>
#include <ncurses.h>
#include <sqlite3.h>

#include "db/card.hpp"
#include "db/cardRepo.hpp"
#include "db/collection.hpp"
#include "db/collectionRepo.hpp"

int main(){
    sqlite3 *db;
    int rc = sqlite3_open("carddata.db", &db);

    if(rc == SQLITE_OK){
        std::string errorMessage = "Error opening the database: quitting";
        std::cerr << errorMessage << std::endl;
        return -1;
    }

    char* zErrMsg = 0;
    //const char* sql = "CREATE TABLE CARDS)

    sqlite3_close(db);
    return 0;
}
