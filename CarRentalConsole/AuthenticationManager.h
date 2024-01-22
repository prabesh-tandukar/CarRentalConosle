#pragma once
#include <string>
#include <sqlite3.h>
#include "IDatabaseOperations.h"

struct AuthResult {
	int userID;
	std::string userRole;
};

class AuthenticationManager
{
private:
	IDatabaseOperations* dbOperations;
public:
	AuthenticationManager(IDatabaseOperations* dbOperations);
	static AuthResult authenticateUser(const std::string& username, const std::string& password);
};

