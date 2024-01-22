// AuthenticationManager.cpp
#include "AuthenticationManager.h"
#include <iostream>

AuthenticationManager::AuthenticationManager(IDatabaseOperations* dbOperations) : dbOperations(dbOperations) {}

AuthResult AuthenticationManager::authenticateUser(const std::string& username, const std::string& password) {
    const char* sql = "SELECT userID, role FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt* stmt;
    AuthResult result = { -1, "" };

    if (sqlite3_prepare_v2(dbOperations->getDB(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
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
