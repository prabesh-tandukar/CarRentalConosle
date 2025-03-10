#include "AdminInterface.h"
#include <iostream>
#include <string>
#include <sqlite3.h>
#include "BookingManager.h"
#include <limits>

AdminInterface::AdminInterface(sqlite3* db) : db(db) {
	//constructore implementation
	//std::cout << "Debug: DB pointer address inside AdminInterface constructor: " << this->db << std::endl;
	
}

void AdminInterface::clearScreen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif // _WIN32
}

bool AdminInterface::showMenu() {
	AdminInterface::clearScreen();
	std::cout << "\n----------Admin Menu---------\n";
	int choice;

	do {
		std::cout << "+---------------------------+\n";
		std::cout << "| 1. Add New Car            |\n";
		std::cout << "| 2. Update Car             |\n";
		std::cout << "| 3. Manage Rental Bookings |\n";
		std::cout << "| 4. Log out                |\n";
		std::cout << "+---------------------------+\n";
		std::cout << "Enter your choice: ";

		if (std::cin >> choice) {
			// Input is successful, clear any potential error flags
			std::cin.clear();

			switch (choice) {
			case 1: {
				addNewCar();
				break;
			}
			case 2: {
				updateCarDetails();
				break;
			}
			case 3: {
				BookingManager bookingManger(db);
				bookingManger.showMenu();
				break;
			}
			case 4: {
				std::cout << "Logging out\n";
				return true;
			}
			default: {
				std::cout << "Invalid Choice. Please Try again\n";
				break;
			}
			}
		}
		else {
			// Input is not an integer, clear the error and discard the invalid input
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid input. Please enter a number.\n";
		}
	} while (true);
}

void AdminInterface::addNewCar() {
	//Check if db is nullptr
	if (db == nullptr) {
		std::cerr << "Database connection is not initialized.\n";
		return;
	}
	std::string make, model;
	int year = 0, mileage= 0, minRentPeriod = 0, maxRentPeriod = 0;
	bool isAvailable = false;


	//Getting car details from the admin
	std::cout << "\n+-----------------------+\n";
	std::cout << "|   Add New Car         |\n";
	std::cout << "+-----------------------+\n";
	std::cout << "Enter Car Make:";
	std::cin >> make;
	std::cout << "Enter Car Model: ";
	std::cin >> model;
	std::cout << "Enter Car Year: ";
	std::cin >> year;
	std::cout << "Enter Mileage: ";
	std::cin >> mileage;
	std::cout << "Is the car available ( 1 for Yes, 0 for No): ";
	std::cin >> isAvailable;
	std::cout << "Enter the Minimum Rent Period (days): ";
	std::cin >> minRentPeriod;
	std::cout << "Enter Maximum Rent Period (days): ";
	std::cin >> maxRentPeriod;
	std::cout << "+-----------------------+\n";


	// Convert bool to int for SQLite
	int isAvailableInt = isAvailable ? 1 : 0;

	//Constructing the SQL INSERT statement
	std::string sql = "INSERT INTO Car (Make, Model, Year, Mileage, IsAvailable, MinRentPeriod, MaxRentPeriod) VALUES ('" + make + "', '" + model + "', " + std::to_string(year) + ", " + std::to_string(mileage) + ", " + std::to_string(isAvailable) + ", " + std::to_string(minRentPeriod) + ", " + std::to_string(maxRentPeriod) + ");";

	//std::cout << "\nExecuting SQL: " << sql << std::endl; //Debugging output

	//Execute SQL staement
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
	else {
		std::cout << "New car added successfully." << std::endl;
	}

}

void AdminInterface::updateCarDetails() {
	if (db == nullptr) {
		std::cerr << "Database connection is not initialized.\n";
		return;
	}

	//Display the list of cars with IDs and makes
	std::cout << "\n+---------------------------------+\n";
	std::cout << "|   List of Cars for Update       |\n";
	std::cout << "+---------------------------------+\n";

	std::string listCarsSQL = "SELECT CarID, Make, Model FROM Car;";
	sqlite3_stmt* listStmt = nullptr;

	if (sqlite3_prepare_v2(db, listCarsSQL.c_str(), -1, &listStmt, nullptr) == SQLITE_OK) {
		int result = sqlite3_step(listStmt);
		if (result == SQLITE_ROW) {
			do {
				std::cout << "CarID: " << sqlite3_column_int(listStmt, 0)
					<< " | Make: " << sqlite3_column_text(listStmt, 1)
					<< " | Model: " << sqlite3_column_text(listStmt, 2)
					<< "\n";
			} while (sqlite3_step(listStmt) == SQLITE_ROW);
		}
		else {
			std::cout << "No cars available for update.\n";
			sqlite3_finalize(listStmt);
			return;
		}
		sqlite3_finalize(listStmt);
	}
	else {
		std::cerr << "Failed to prepare the list of cars statement.\n";
		return;
	}

	std::cout << "+--------------------------------------+\n";

	int carID;
	std::cout << "Enter the ID of the car you want to update: ";
	if (!(std::cin >> carID)) {
		std::cerr << "Invalid input for car ID .\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return;
	};

	//You might want to fetch and display the current details of the car here
	
	std::string make, model, tempInput;
	int year = 0, mileage = 0, minRentPeriod = 0, maxRentPeriod = 0;
	bool isAvailable = false;

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //Clear the input buffer

	//Getting updated car details from the admin
	std::cout << "Enter new Car Make (leave blank to keep current): ";
	std::getline(std::cin, make);

	std::cout << "Enter new Car Model (leave blank to keep current): ";
	std::getline(std::cin, model);

	std::cout << "Enter new Car Year (leave blank to keep current): ";
	std::getline(std::cin, tempInput);
	if (!tempInput.empty()) {
		try {
			year = std::stoi(tempInput); //Convert string to int
		}
		catch (...) {
			std::cerr << "Invalid input for year.\n";
			return;
		}
		
	}

	std::cout << "Enter new Mileage (leave blank to keep current): ";
	std::getline(std::cin, tempInput);
	if (!tempInput.empty()) {
		try {
			mileage = std::stoi(tempInput);
		}
		catch (...) {
			std::cerr << "Invalid input for mileage.\n";
			return;
		}
	}

	std::cout << "Is the car available (1 for Yes, 0 for No, leave blank to keep current): ";
	std::getline(std::cin, tempInput);
	if (!tempInput.empty()) {
		isAvailable = tempInput == "1";
		if (tempInput != "1" && tempInput != "0") {
			std::cerr << "Invalid input for availability. Enter 1 or 0.\n";
			return;
		}
	}

	std::cout << "Enter the Minimum Rent Period (days, leave blank to keep current): ";
	std::getline(std::cin, tempInput);
	if (!tempInput.empty()) {
		try {
			minRentPeriod = std::stoi(tempInput);
		}
		catch (...) {
			std::cerr << "Invalid input for minimum rent period.\n";
			return;
		}
	}


	std::cout << "Enter Maximum Rent Period (days, leave blank to keep current): ";
	std::getline(std::cin, tempInput);
	if (!tempInput.empty()) {
		try {
			maxRentPeriod = std::stoi(tempInput);
		}
		catch (...) {
			std::cerr << "Invalid input for maximum rent period.\n";
			return;
		}
	}

	// Construct the SQL UPDATE statement
	std::string sql = "UPDATE Car SET ";
	sql += !make.empty() ? "Make = '" + make + "', " : "";
	sql += !model.empty() ? "Model = '" + model + "', " : "";
	sql += (year > 0) ? "Year = " + std::to_string(year) + ", " : "";
	sql += (mileage > 0) ? "Mileage = " + std::to_string(mileage) + ", " : "";
	sql += "IsAvailable = " + std::to_string(isAvailable ? 1 : 0);
	sql += (minRentPeriod > 0) ? ", MinRentPeriod = " + std::to_string(minRentPeriod) : "";
	sql += (maxRentPeriod > 0) ? ", MaxRentPeriod = " + std::to_string(maxRentPeriod) : "";
	sql += " WHERE CarID = " + std::to_string(carID) + ";";


	// Execute SQL statement
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
	else {
		std::cout << "Car details updated successfully.\n";
	}
	std::cout << "+-----------------------------+\n";
}