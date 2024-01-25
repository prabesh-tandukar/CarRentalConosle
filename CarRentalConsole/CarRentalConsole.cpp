// CarRentalConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <string>
#include <sqlite3.h>
#include "AdminInterface.h";
#include "UserInterface.h";
#include "BookingManager.h";
#include "DatabaseManager.h";
#include "PasswordManager.h";
#include <cstdlib>
#include <conio.h>


//SQLite database file
const char* DB_FILE = "car.db";

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif // _WIN32
}


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

void createDummyData(sqlite3* db) {
    // Check if the Car table is empty
    const char* checkCarTableEmptySQL = "SELECT COUNT(*) FROM Car;";
    sqlite3_stmt* checkCarTableStmt;
    int carRowCount = 0;

    if (sqlite3_prepare_v2(db, checkCarTableEmptySQL, -1, &checkCarTableStmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(checkCarTableStmt) == SQLITE_ROW) {
            carRowCount = sqlite3_column_int(checkCarTableStmt, 0);
        }
        sqlite3_finalize(checkCarTableStmt);
    }

    // If the Car table is empty, insert dummy data
    if (carRowCount == 0) {
        // Role table dummy data
        const char* insertRoleDummyDataSQL = "INSERT INTO Role (RoleName) VALUES "
            "('user'),"
            "('admin');";
        executeQuery(db, insertRoleDummyDataSQL);

        // Car table dummy data
        const char* insertCarDummyDataSQL = "INSERT INTO Car (Make, Model, Year, Mileage, IsAvailable, MinRentPeriod, MaxRentPeriod) VALUES "
            "('Toyota', 'Corolla', 2022, 10000, 1, 1, 7),"
            "('Honda', 'Civic', 2021, 12000, 1, 1, 7),"
            "('Ford', 'Focus', 2022, 8000, 1, 1, 7),"
            "('Chevrolet', 'Malibu', 2021, 15000, 1, 1, 7),"
            "('BMW', 'X5', 2020, 20000, 1, 1, 7),"
            "('Mercedes', 'C-Class', 2021, 18000, 1, 1, 7),"
            "('Audi', 'A4', 2022, 16000, 1, 1, 7),"
            "('Nissan', 'Altima', 2020, 13000, 1, 1, 7),"
            "('Hyundai', 'Elantra', 2022, 11000, 1, 1, 7),"
            "('Kia', 'Optima', 2021, 14000, 1, 1, 7);";
        executeQuery(db, insertCarDummyDataSQL);

        std::cout << "Dummy data insertion completed.\n";
    }
   
}


//Function to register a user
void registerUser(sqlite3* db, const std::string& username, unsigned long hashedPassword) {
    std::string defaultRole = "user"; //Default role for new users
    std::string sql = "INSERT INTO users (username, password, role) VALUES (?, ?, ?); ";
    //int rc = executeQuery(db, sql.c_str());
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(hashedPassword));
        sqlite3_bind_text(stmt, 3, defaultRole.c_str(), -1, SQLITE_STATIC);

        //Execute the statement
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            std::cout << "User regsitered successfully. \n";
        }
        else {
            std::cerr << "Failed to register user. \n";
        }

        //Finalize the statement to release resources
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "Failed to prepare the SQL statment. \n";
    }
}

void registerView(sqlite3* db) {
    clearScreen();
    std::cout << "********** WELCOME TO THE REGISTER PAGE **********" << std::endl;
    std::string newUsername, newPassword;

    //Prompt for username
    std::cout << "Enter your username: ";
    std::cin >> newUsername;

    //Prompt for password
    newPassword = PasswordManager::promptForPassword();

    //Hash the entered password
    unsigned long hashedPassword = PasswordManager::djb2Hash(newPassword);

    registerUser(db, newUsername, hashedPassword);
    // Provide feedback to the user
    clearScreen();
    std::cout << "********** REGISTRATION SUCCESSFUL **********" << std::endl;
    std::cout << "Thank you for registering, " << newUsername << "!" << std::endl;
    std::cout << "You can now log in using your credentials." << std::endl;
}

struct AuthResult {
    int userID;
    std::string userRole;
};

AuthResult authenticateUser(sqlite3* db, const std::string& username,unsigned long hashedPassword) {
    const char* sql = "SELECT userID, role FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt* stmt;
    AuthResult result = { -1, "" };

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(hashedPassword));

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
    //bool success = false;
    clearScreen();
    std::cout << "********** WELCOME TO THE LOGIN PAGE **********" << std::endl;
    std::string authUsername, authPassword;
    std::cout << "Enter your username: ";
    std::cin >> authUsername;
    // Prompt for password using PasswordManager
    authPassword = PasswordManager::promptForPassword();

    // Hash the entered password
    unsigned long hashedPassword = PasswordManager::djb2Hash(authPassword);



    return authenticateUser(db, authUsername, hashedPassword);
}

void promptForPassword(std::string& password) {
    char ch;
    std::cout << "Enter your password";

    while (true) {
        ch = _getch();

        if (ch = 13) //enter key
            break;

        password.push_back(ch);
        std::cout << "*";
    }
    std::cout << std::endl;
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



void printSeparator() {
    std::cout << "----------------------------------------" << std::endl;
}

void printMenuOption(int option, const std::string& description) {
    std::cout << " " << option << ". " << description << std::endl;
}

void printMainMenu() {
    //clearScreen();
    std::cout << "********** Welcome to Easy Car Rental **********" << std::endl;
    printSeparator();
    printMenuOption(1, "Login");
    printMenuOption(2, "Register");
    printMenuOption(3, "Exit");
    printSeparator();
    std::cout << "Enter your choice: ";
}



int main()
{
    sqlite3* db;
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    createTables(db);

    //Check if admin exists and if not, create one
    if (!adminExists(db)) {
        unsigned long hashedPassword = PasswordManager::djb2Hash("admin");

        std::string adminSQL = "INSERT INTO users(username, password, role) VALUES ('admin', '" + std::to_string(hashedPassword) + "', 'admin');";
        executeQuery(db, adminSQL.c_str());
    }

    createDummyData(db);
    
    bool appRunning = true;
    int userID = 1;
    std::string role;
    
    while (appRunning) {
        printMainMenu();
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


