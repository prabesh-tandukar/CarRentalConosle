#pragma once
#include <string>
#include <sqlite3.h>
#include "IDatabaseOperations.h"

class ApplicationManager
{
private:
	IDatabaseOperations* dbOperations;
public:
	ApplicationManager(IDatabaseOperations* dbOperation);
	static void runApplication(sqlite3* db, int userID, const std::string& role);
};

