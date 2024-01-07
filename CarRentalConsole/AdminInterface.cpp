#include "AdminInterface.h"
#include <iostream>
#include <string>

AdminInterface::AdminInterface(sqlite3* db) {
	//constructore implementation
	showMenu();
}

void AdminInterface::showMenu() {
	std::cout << "\n ---Admin Menu ---\n";
	//Implementation of admin menu
	int input{ 0 };
	std::cout << "1: Add New Car:" << std::endl;
	std::cout << "2: Update Car:" << std::endl;
	std::cout << "3: Manage Rental Bookings\n" << std::endl;
	std::cin >> input;
	if (input == 1) {
		addNewCar();
	}

}

void AdminInterface::addNewCar() {
	std::string make, model;
	int year, mileage;
	bool isAvailable;
	int minRentPeriod, maxRentPeriod;

	//Getting car details from the admin
	std::cout << "Enter Car Make:";
	std::cin >> make;
	std::cout << "Enter Car Model: ";
	std::cin >> model;
	std::cout << "Enter Car Year: ";
	std::cin >> year;
	std::cout << "Enter Mileage";
	std::cin >> mileage;
	std::cout << "Is the car available ( 1 for Yes, 0 for No): ";
	std::cin >> isAvailable;
	std::cout << "Enter the Minimum Rent Period (days): ";
	std::cin >> minRentPeriod;
	std::cout << "Enter Maximum Rent Period (days): ";
	std::cin >> maxRentPeriod;

	//Constructing the SQL INSERT statement
	std::string sql = "INSERT INTO Car (Make, Model, Year, Mileage, IsAvailable, MinRentPeriod, MaxRentPeriod) VALUES ('" + make + "', '" + model + "', " + std::to_string(year) + ", " + std::to_string(mileage) + ", " + std::to_string(isAvailable) + ", " + std::to_string(minRentPeriod) + ", " + std::to_string(maxRentPeriod) + ");";

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