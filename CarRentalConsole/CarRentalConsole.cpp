// CarRentalConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <string>
#include <sqlite3.h>
#include "AdminInterface.h";
#include "UserInterface.h";

//SQLite database file
const char* DB_FILE = "car.db";

// Callback function for executing SQL statements
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << "------------------------" << std::endl;
    return 0;
}

//Function to execute SQL queries
int executeQuery(sqlite3* db, const char* sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, callback, 0, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    return rc;
}

//Function to register a user
void registerUser(sqlite3* db, const std::string& username, const std::string& password) {
    std::string sql = "INSERT INTO users (username, password, role) VALUES ('" + username + "', '" + password + "' , 'user' );";
    int rc = executeQuery(db, sql.c_str());

    if (rc == SQLITE_OK) {
        std::cout << "User registered successfully. " << std::endl;
    }
    else {
        std::cerr << "Failed to register user. " << std::endl;
    }
}

void registerView(sqlite3* db) {
    std::cout << "-----WELCOME TO THE REGISTER PAGE-----";
    std::string newUsername, newPassword;
    std::cout << "Enter your username: ";
    std::cin >> newUsername;
    std::cout << "Enter your password: ";
    std::cin >> newPassword;

    registerUser(db, newUsername, newPassword);
}

std::string authenticateUser(sqlite3* db, const std::string& username, const std::string& password) {
    const char* sql = "SELECT role FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt* stmt;
    std::string userRole = "";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* role = sqlite3_column_text(stmt, 0);
            if (role != nullptr) {
                userRole = reinterpret_cast<const char*>(role);
            }
        }
        else {
            std::cerr << "Authentication failed. Invalid username or password." << std::endl;
        }
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "Failed to execute query." << std::endl;
    }

    return userRole;
}
 

std::string authenticateView(sqlite3* db) {
    bool success = false;
    std::cout << "-----WELCOME TO THE LOGIN PAGE-----";
    std::string authUsername, authPassword;
    std::cout << "Enter your username: ";
    std::cin >> authUsername;
    std::cout << "Enter your password: ";
    std::cin >> authPassword;

    return authenticateUser(db, authUsername, authPassword);
}

bool adminExists(sqlite3* db) {
    const char* adminCheckSql = "SELECT * FROM users WHERE username='admin'";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, adminCheckSql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}



int main()
{

    sqlite3* db;
    int rc = sqlite3_open(DB_FILE, &db);
    int input;
    bool loginSuccess = false;
    


    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    //drop user table
    /*std::string dropsql = "DROP TABLE users";
    executeQuery(db, dropsql.c_str());*/

    //View data in user table
    std::string selectsql = "SELECT * FROM USERS";
    int outp = executeQuery(db, selectsql.c_str());
  

    // Create users table if it doesn't exist
    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT, role TEXT);";
    rc = executeQuery(db, createTableSQL);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to create table." << std::endl;
        return rc;
    }

    //Check if admin exists and if not, create one
    if (!adminExists(db)) {
        std::string adminSQL = "INSERT INTO users(username, password, role) VALUES ('admin', 'admin', 'admin');";
        executeQuery(db, adminSQL.c_str());
    }

    bool appRunning = true;
    while (appRunning) {
        std::cout << "Welcome to Easy Car Rental:" << std::endl;
        std::cout << "Enter 1 to login and 2 to register, 3 to exit" << std::endl;
        std::cin >> input;

        if (input == 1) {
            std::string role = authenticateView(db);
            if (role == "admin") {
                //Show admin interface
                AdminInterface adminInterface(db);
                adminInterface.showMenu();
            }
            else if (role == "user") {
                //Show user interface
                UserInterface userInterface(db);
                userInterface.showMenu();
            }
            else {
                std::cout << "Authentication failed.\n";
            }
        }
        else if (input == 2) {
            registerView(db);
        }
        else if (input == 3) {
            appRunning = false;
        }
        else {
            std::cout << "Invalid input. Please try again.\n";
        }
    }
    // Close the database
    sqlite3_close(db);
    return 0;
}


