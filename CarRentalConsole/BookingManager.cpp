#include "BookingManager.h"
#include <iostream>

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	for (int i = 0; i < argc; i++) {
		std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL" ) << std::endl;
	}
	std::cout << "-------------------------------------------" << std::endl;
	return 0;
}

void BookingManager::viewAllBookings() {
	std::string sql = "SELECT * FROM RentalBooking;";

	//execute this sql and display the results
	char* errMsg = nullptr;
	int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg
	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error" << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
	return rc;
	//The "callback" function should be defined to handle the rows returned by the query.
}
