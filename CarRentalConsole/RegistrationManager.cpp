// RegistrationManager.cpp
#include "RegistrationManager.h"

void RegistrationManager::registerUser(IDatabaseOperations* dbOperations, const std::string& username, const std::string& password) {
    std::string defaultRole = "user"; // Default role for new users
    std::string sql = "INSERT INTO users (username, password, role) VALUES ('" + username + "', '" + password + "' , '" + defaultRole + "' ); ";
    int rc = dbOperations->executeQuery(sql.c_str());

    if (rc == SQLITE_OK) {
        std::cout << "User registered successfully. " << std::endl;
    }
    else {
        std::cerr << "Failed to register user. " << std::endl;
    }
}
