#pragma once
#include <sqlite3.h>
class dbConnection
{
public:
	int callback(void* NotUsed, int argc, char** argv, char** azColName) {
		
	}

	void createDb(sqlite3** db, char* zErrMsg, int rc){
		
	}

	
	
};

