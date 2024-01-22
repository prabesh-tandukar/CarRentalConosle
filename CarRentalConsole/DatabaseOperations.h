#pragma once
#include "IDatabaseOperations.h"

class DatabaseOperations : public IDatabaseOperations
{
private:
	sqlite3* db;
public:
	DatabaseOperations(sqlite3* db);
	int executeQuery(const std::string& sql) override;
};

