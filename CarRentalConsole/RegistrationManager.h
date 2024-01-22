#pragma once
#include <string>
#include <sqlite3.h>

class RegistrationManager
{
public:
	static void registerUser(sqlite3* db, const std::string& username, const std::string& password);
};

