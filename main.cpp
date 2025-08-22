#include <iostream>
#include <sqlite3.h>

#include "src/db/card.hpp"
#include "src/db/cardRepo.hpp"
#include "src/db/collection.hpp"
#include "src/db/collectionRepo.hpp"

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
