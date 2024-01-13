#pragma once
#include <sqlite3.h>
#include <string>

class DatabaseManager
{
public:
	int getUserId(sqlite3* db, const std::string& username);
};

