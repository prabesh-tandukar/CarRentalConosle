#pragma once
#include <string>
#include <sqlite3.h>

class IDatabaseOperations
{
public: 
	virtual int executeQuery(const std::string& sql) = 0;
	virtual ~IDatabaseOperations() = default;
	virtual sqlite3* getDB() const = 0;
};

