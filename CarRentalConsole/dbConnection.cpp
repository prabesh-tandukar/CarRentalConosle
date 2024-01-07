#include "dbConnection.h"
#include <sqlite3.h>
#include <iostream>

char* zErrMsg = 0;
int rc;

// Callback function for executing SQL statements
int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    return 0;
}

void createDb(sqlite3** db, char* zErrMsg, int rc) {
    *rc = sqlite3_open("car.db", &db);

    if (rc) {
        std::cerr << "Canlt open database: " << sqlite3_errmsg(db) << std::endl;
        //return rc;
    }
    else {
        std::cout << "Opened database successfully" << std::endl;
    }

    const char* createTableSQL = "CREATE TABLE USERS("
        "ID INT PRIMARY KEY NOT NULL,"
        "NAME            TEXT NOT NULL,"
        "PASSWORD TEXT NOT NULL,"
        "ADMIN BOOL,);";

    rc = sqlite3_exec(db, createTableSQL, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
    }
    else {
        std::cout << "Table created successfully" << std::endl;
    }

    sqlite3_close(db);
}
