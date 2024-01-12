#include "BookingManager.h"
#include <iostream>

BookingManager::BookingManager(sqlite3* db) : db(db) {
	//constructor 
}

int BookingManager::callback(void* NotUsed, int argc, char** argv, char** azColName) {
	for (int i = 0; i < argc; i++) {
		std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << "\n";
	}
	std::cout << "\n";
	return 0;
}

void BookingManager::showMenu() {
	int choice;
	std::cout << "1: View All Bookings \n 2: Update Booking \n 3: Cancel Booking \n 4: Approve Booking \n 5: Reject Booking \n";
	std::cin >> choice;

	switch (choice) {
	case 1:
		viewAllBookings();
		break;
	case 2:
		updateBooking();
		break;
	case 3:
		cancelBooking();
		break;
	case 4:
		approveBooking();
		break;
	case 5:
		rejectBooking();
		break;
	default: std::cout << "Invalid choice. \n";
	}
}

void BookingManager::viewAllBookings() {
	const char* sql = "SELECT * FROM RentalBooking;";
	char* errMsg = nullptr;

	
	if (sqlite3_exec(db, sql, callback, 0, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error" << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

void BookingManager::updateBooking() {
	int bookingId;
	std::string newStartDate, newEndDate, tempInput;
	double totalCost = -1; //Using -1 as default value to indicate "no update"

	std::cout << "Enter Booking ID to update: ";
	std::cin >> bookingId;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //Clear the input buffer

	std::cout << "Enter new start date (YYYY-MM-DD, leave blank to keep current): ";
	std::getline(std::cin, newStartDate);

	std::cout << "Enter new end date (YYYY-MM-DD, leave blank to keep current): ";
	std::getline(std::cin, newEndDate);

	std::cout << "Enter new total cost (leave blank to keep current): ";
	std::getline(std::cin, tempInput);
	if (!tempInput.empty()) {
		try {
			totalCost = std::stod(tempInput); // Convert string to double
		}
		catch (...) {
			std::cerr << "Invalid input for total cost.\n";
			return;
		}
	}

	std::string sql = "UPDATE RentalBooking SET ";
	sql += !newStartDate.empty() ? "StartDate = '" + newStartDate + "', " : "";
	sql += !newEndDate.empty() ? "EndDate = '" + newEndDate + "', " : "";
	if (totalCost != -1) {
		sql += "TotalCost = " + std::to_string(totalCost) + ", ";
	}
	sql.pop_back(); 
	sql.pop_back();
	sql += " WHERE BookingId = " + std::to_string(bookingId) + ";";
	

	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
	else {
		std::cout << "Booking updated successfully.\n";
	}
}

void BookingManager::cancelBooking() {
	int bookingId;
	std::cout << "Enter Booking ID to cancel: ";
	std::cin >> bookingId;

	std::string sql = "DELETE FROM RentalBooking WHERE BookingID = " + std::to_string(bookingId) + ";";
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
	else {
		std::cout << "Booking canceled successfully.\n";
	}
}

void BookingManager::approveBooking() {
	int bookingId;
	std::cout << "Enter Booking ID to approve: ";
	std::cin >> bookingId;

	std::string sql = "UPDATE RentalBooking SET Status = 'Approved' WHERE BookingID = " + std::to_string(bookingId) + ";";
	// Execute the SQL query...
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
	else {
		std::cout << "Booking approved successfully.\n";
	}
}

void BookingManager::rejectBooking() {
	int bookingId;
	std::cout << "Enter Booking ID to reject: ";
	std::cin >> bookingId;

	std::string sql = "UPDATE RentalBooking SET Status = 'Rejected' WHERE BookingID = " + std::to_string(bookingId) + ";";
	// Execute the SQL query...
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
	else {
		std::cout << "Booking rejected successfully.\n";
	}
}
