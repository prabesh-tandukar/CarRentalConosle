// CarRentalConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <string>
#include <sqlite3.h>
#include "AdminInterface.h";
#include "UserInterface.h";
#include "BookingManager.h";
#include "DatabaseManager.h"

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

void createTables(sqlite3* db) {
    const char* sqlUserTable = "CREATE TABLE IF NOT EXISTS Users (UserID INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT, role TEXT DEFAULT 'user');";
    
    const char* sqlRoleTable = "CREATE TABLE IF NOT EXISTS Role (RoleID INTEGER PRIMARY KEY AUTOINCREMENT, RoleName TEXT NOT NULL);";

    const char* sqlCarTable = "CREATE TABLE IF NOT EXISTS Car (CarID INTEGER PRIMARY KEY AUTOINCREMENT, Make TEXT, Model TEXT, Year INTEGER, Mileage INTEGER, IsAvailable BOOLEAN, MinRentPeriod INTEGER, MaxRentPeriod INTEGER);";

    const char* sqlRentalBookingTable = "CREATE TABLE IF NOT EXISTS RentalBooking (BookingID INTEGER PRIMARY KEY AUTOINCREMENT, CustomerID INTEGER, CarID INTEGER, StartDate TEXT, EndDate TEXT, TotalCost REAL, Status TEXT DEFAULT 'Pending', FOREIGN KEY (CustomerID) REFERENCES User(UserID), FOREIGN KEY (CarID) REFERENCES Car(CarID));";

    executeQuery(db, sqlUserTable);
    executeQuery(db, sqlRoleTable);
    executeQuery(db, sqlCarTable);
    executeQuery(db, sqlRentalBookingTable);
}

//Function to register a user
void registerUser(sqlite3* db, const std::string& username, const std::string& password) {
    std::string defaultRole = "user"; //Default role for new users
    std::string sql = "INSERT INTO users (username, password, role) VALUES ('" + username + "', '" + password + "' , '" + defaultRole + "' ); ";
    int rc = executeQuery(db, sql.c_str());

    if (rc == SQLITE_OK) {
        std::cout << "User registered successfully. " << std::endl;
    }
    else {
        std::cerr << "Failed to register user. " << std::endl;
    }
}

void registerView(sqlite3* db) {
    std::cout << "-------WELCOME TO THE REGISTER PAGE------" << std::endl;
    std::string newUsername, newPassword;
    std::cout << "Enter your username: ";
    std::cin >> newUsername;
    std::cout << "Enter your password: ";
    std::cin >> newPassword;

    registerUser(db, newUsername, newPassword);
}

struct AuthResult {
    int userID;
    std::string userRole;
};

AuthResult authenticateUser(sqlite3* db, const std::string& username, const std::string& password) {
    const char* sql = "SELECT userID, role FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt* stmt;
    AuthResult result = { -1, "" };

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result.userID = sqlite3_column_int(stmt, 0);
            const unsigned char* role = sqlite3_column_text(stmt, 1);
            if (role != nullptr) {
                result.userRole = reinterpret_cast<const char*>(role);
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

    return result;
}
 

AuthResult authenticateView(sqlite3* db) {
    bool success = false;
    std::cout << "-*-*-*-*-WELCOME TO THE LOGIN PAGE-*-*-*-*-" << std::endl;
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

//void insertTestData(sqlite3* db) {
//    const char* sql = "INSERT INTO RentalBooking (CustomerID, CarID, StartDate, EndDate, TotalCost, Status) VALUES (1, 1, '2023-01-01', '2023-01-10', 100.00, 'Confirmed');";
//    char* errMsg = nullptr;
//    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
//        std::cerr << "SQL Error: " << errMsg << std::endl;
//        sqlite3_free(errMsg);
//    }
//    else {
//        std::cout << "Test data inserted successfully.\n";
//    }
//}




int main()
{
    sqlite3* db;
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    createTables(db);

    //drop user table
    /*std::string dropsql = "DROP TABLE users";
    executeQuery(db, dropsql.c_str());*/
    /*std::string dropsql = "DROP TABLE rentalbooking";
    executeQuery(db, dropsql.c_str());*/

    //View data in user table
    std::string selectsql = "SELECT * FROM USERS";
    int outp = executeQuery(db, selectsql.c_str());

    //View data in car table
    std::string carsql = "SELECT * FROM CAR";
    executeQuery(db, carsql.c_str());
    std::string booksql = "SELECT * FROM RENTALBOOKING";
    executeQuery(db, booksql.c_str());
  
    //insertTestData(db);

    //Check if admin exists and if not, create one
    if (!adminExists(db)) {
        std::string adminSQL = "INSERT INTO users(username, password, role) VALUES ('admin', 'admin', 'admin');";
        executeQuery(db, adminSQL.c_str());
    }

    
    bool appRunning = true;
    int userID = 1;
    std::string role;
    
    while (appRunning) {
        std::cout << "----------Welcome to Easy Car Rental----------:" << std::endl;
        std::cout << "Enter: \n 1 to login \n 2 to register \n 3 to exit" << std::endl;
        int input;
        std::cin >> input;

        if (input == 1) {
            AuthResult authResult = authenticateView(db);
            userID = authResult.userID;
            role = authResult.userRole;
            if (role == "admin") {
                //Show admin interface
                AdminInterface adminInterface(db);
                adminInterface.showMenu();
            }
            else if (role == "user") {
                
                //std::cout << "The logged in user is " << userID;
                //Show user interface
                UserInterface userInterface(db, userID);
                if (userInterface.showMenu()) {
                    userID = -1;
                    role = "";
                }
            }
            else {
                std::cout << "Authentication failed.\n";
            }
        }
        else if (input == 2) {
            registerView(db);
        }
        else if (input == 3) {
            std::cout << "Exiting....\n";
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


