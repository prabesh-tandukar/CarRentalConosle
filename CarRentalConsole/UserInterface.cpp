#include "UserInterface.h"
#include <iostream>
#include <string>
#include <sqlite3.h>


static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	for (int i = 0; i < argc; i++) {
		std::cout << azColName[i] << " : " << (argv[i] ? argv[i] : "NULL") << "\n";
	}
	std::cout << "--------------------------------------\n";
	return 0;
}

UserInterface::UserInterface(sqlite3* db, int userID) : db(db), userID(userID) {
	//Constructor implementation
}

bool UserInterface::showMenu() {
	int choice;
	do {
		std::cout << "User Menu :\n1. View Available Cars\n2. View Bookings\n3. Book a car\n4. Log Out\n";
		std::cin >> choice;
		//Implementation of user menu

		switch (choice) {
		case 1:
			viewAvailableCars();
			break;
		case 2:
			viewUserBookings();
			break;
		case 3:
			bookCar();
			break;
		case 4:
			std::cout << "Loggin out....\n";
			return true;
		default:
			std::cout << "Invalid Choice.Please Try again";
			break;
		}
	} while (true);
}

void UserInterface::viewAvailableCars() {
	std::string sql = "SELECT CarID, Make, Model, IsAvailable FROM Car WHERE IsAvailable = 1;";

	char* errMsg = nullptr;

	if (sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

void UserInterface::viewCarDetails() {

}

bool UserInterface::validateCarID(int carID) {
	std::string sql = "SELECT * FROM Car WHERE CarID = ? AND IsAvailable = 1;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, carID);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			//CarID is valid (exists and is available)
			sqlite3_finalize(stmt);
			return true;
		}
		else {
			//CarID is invalid (does not exists or is not available)
			std::cerr << "Invalid CarID. Please choose an available car.\n";
		}

		sqlite3_finalize(stmt);
	}
	else {
		std::cerr << "Failed to prepare the car valiation statement.\n";
	}
	return false;
}

void UserInterface::bookCar() {
	int carID;
	std::string startDate, endDate;

	//Display available cars first
	viewAvailableCars();

	std::cout << "Enter the CarID you want to book: ";
	std::cin >> carID;

	//validate if the carID is valid (exists and is available)
	if (!validateCarID(carID)) {
		std::cout << "Booking aborted. Please choose a valid and available car.\n";
		return;
	}

	std::cout << "Enter the strat date (YYYY-MM-DD): ";
	std::cin >> startDate;

	std::cout << "Enter the end date (YYYY-MM-DD): ";
	std::cin >> endDate;

	//Perform the booking
	std::string insertBookingSQL = "INSERT INTO RentalBooking (CustomerID, CarID, StartDate, EndDate, Status) VALUES (?, ?, ?, ?, 'Pending');";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, insertBookingSQL.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, userID);
		sqlite3_bind_int(stmt, 2, carID);
		sqlite3_bind_text(stmt, 3, startDate.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, endDate.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			std::cout << "Booking successful. Your request is pending approval.\n";
		}
		else {
			std::cerr << "Booking failed. Please try again.\n";
		}
		sqlite3_finalize(stmt);
	}
	else {
		std::cerr << "Failed to prepare the booking statement.\n";
	}
}



void UserInterface::viewUserBookings() {

	int choice;
	std::string statusFilter;

	std::cout << "Select booking type:\n";
	std::cout << "1. All Bookings\n";
	std::cout << "2. Pending Bookings\n";
	std::cout << "3. Confirmed Bookings\n";
	std::cout << "4. Rejected Bookings\n";
	std::cin >> choice;

	switch (choice) {
	case 1:
		statusFilter = ""; // All bookings
		break;
	case 2:
		statusFilter = "Pending";
		break;
	case 3:
		statusFilter = "Confirmed";
		break;
	case 4:
		statusFilter = "Rejected";
		break;
	default:
		std::cout << "Invalid choice.\n";
		return;
	}

	// Now call the function that actually retrieves and shows the bookings
   // with the specified filter
	retrieveAndShowBookings(statusFilter);
}

void UserInterface::retrieveAndShowBookings(const std::string& statusFilter) {
	std::string sql = "SELECT * FROM RentalBooking WHERE CustomerID = " + std::to_string(userID);
	if (!statusFilter.empty()) {
		sql += " AND Status = '" + statusFilter + "'";
	}
	sql += ";";
	char* errMsg = nullptr;

	if (sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

