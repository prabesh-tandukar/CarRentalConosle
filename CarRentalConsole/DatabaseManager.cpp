#include "DatabaseManager.h"
#include <iostream>

int getUserID(sqlite3* db, const std::string& username) {
	sqlite3_stmt* stmt;
	const char* sql = "SELECT id FROM Users WHERE username = ?";
	int userID = -1; //Default to a invalid user ID

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			userID = sqlite3_column_int(stmt, 0);
		}

		sqlite3_finalize(stmt);
	}
	else {
		std::cerr << "Failed to prepare statment: " << sqlite3_errmsg(db) << std::endl;
	}

	return userID;
}
