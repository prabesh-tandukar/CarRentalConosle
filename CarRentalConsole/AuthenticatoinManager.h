#pragma once
#include <string>
#include <sqlite3.h>

struct AuthResult {
	int userID;
	std::string userRole;
};

class AuthenticatoinManager
{
};

