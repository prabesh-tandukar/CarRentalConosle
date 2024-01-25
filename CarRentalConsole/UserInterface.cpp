#include "UserInterface.h"
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>
#include "Car.h"
#include "CarManager.h"
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>


UserInterface::UserInterface(sqlite3* db, int userID) : db(db), userID(userID), carManager(db) {
	//Constructor implementation
}
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	for (int i = 0; i < argc; i++) {
		std::cout << azColName[i] << " : " << (argv[i] ? argv[i] : "NULL") << "\n";
	}
	std::cout << "--------------------------------------\n";
	return 0;
}

bool UserInterface::isValidDate(const std::string& dateStr) {
	std::tm tm = {};
	std::istringstream ss(dateStr);
	ss >> std::get_time(&tm, "%Y-%m-%d");
	return !ss.fail();
}

void UserInterface::clearScreen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif // _WIN32

}

void UserInterface::showMenuHeader() {
	std::cout << "==========================================\n";
	std::cout << "           User Menu\n";
	std::cout << "==========================================\n";
}

bool UserInterface::showMenu() {
	int choice;
	do {
		//clearScreen();
		showMenuHeader();
		std::cout << "1. View Available Cars\n"
			<< "2. View Bookings\n"
			<< "3. Book a Car\n"
			<< "4. Cancel Booking\n"
			<< "5. Search for Cars\n"
			<< "6. Log Out\n";
		std::cout << "Enter your choice: ";
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
			cancelBooking();
			break;
		case 5:
			searchCars();
			break;
		case 6:
			std::cout << "Loggin out....\n";
			return true;
		default:
			std::cout << "Invalid Choice.Please Try again";
			break;
		}
	} while (true);
}

void UserInterface::viewAvailableCarsHeader() {
	std::cout << "==========================================\n";
	std::cout << "         View Available Cars\n";
	std::cout << "==========================================\n";
}

void UserInterface::viewAvailableCars() {
	std::string sql = "SELECT CarID, Make, Model, IsAvailable FROM Car WHERE IsAvailable = 1;";

	char* errMsg = nullptr;

	if (sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
	std::cout << "==========================================\n";
}

void UserInterface::validateCarIDHeader() {
	std::cout << "==========================================\n";
	std::cout << "          Validate CarID\n";
	std::cout << "==========================================\n";
}

bool UserInterface::validateCarID(int carID) {
	validateCarIDHeader();
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

void UserInterface::bookCarHeader() {
	std::cout << "==========================================\n";
	std::cout << "             Book a Car\n";
	std::cout << "==========================================\n";
}

void UserInterface::bookCar() {
	bookCarHeader();
	int carID;
	std::string startDateStr, endDateStr;

	//Display available cars first
	viewAvailableCars();

	std::tm startDate = {};
	std::tm endDate = {};

	std::cout << "Enter the CarID you want to book: ";
	std::cin >> carID;

	//validate if the carID is valid (exists and is available)
	if (!validateCarID(carID)) {
		std::cout << "Booking aborted. Please choose a valid and available car.\n";
		return;
	}

	//Function to get valid year within the range 2024-2030
	auto getValidYear = [](const std::string& prompt) -> int {
		int year;
		do {
			std::cout << prompt;
			std::cin >> year;
		} while (year < 2024 || year > 2030);
		return year;
		};

	//Funtion to get valid month within the range 1-12
	auto getValidMonth = [](const std::string& prompt) -> int {
		int month;
		do {
			std::cout << prompt;
			std::cin >> month;
		} while (month < 1 || month > 12);
		return month;
		};

	//Funtion to get valid day within the range 1-31
	auto getValidDay = [](const std::string& prompt, int maxDay) -> int {
		int day;
		do {
			std::cout << prompt;
			std::cin >> day;
		} while (day < 1 || day > maxDay);
		return day;
		};

	//Get the start date components
	startDate.tm_year = getValidYear("Enter the start year (2024-2030): ") - 1900;
	startDate.tm_mon = getValidMonth("Enter the start month (1-12): ") - 1; //Month is 0-based
	startDate.tm_mday = getValidDay("Enter the start day (1-31): ", 31);

	//Get the end date components
	endDate.tm_year = getValidYear("Enter the end year (2024-2030): ") - 1900;
	endDate.tm_mon = getValidMonth("Enter the end month (1-12): ") - 1; //Month is 0-based
	endDate.tm_mday = getValidDay("Enter the end day (1-31): ", 31);

	//Convert tm sstructres to strings
	std::ostringstream startDateStreamStr;
	std::ostringstream endDateStreamStr;

	startDateStreamStr << std::put_time(&startDate, "%Y-%m-%d");
	endDateStreamStr << std::put_time(&endDate, "%Y-%m-%d");

	startDateStr = startDateStreamStr.str();
	endDateStr = endDateStreamStr.str();

	
	//Calculate the duration between start and end dates
		 auto start = std::chrono::system_clock::from_time_t(std::mktime(&startDate));
		 auto end = std::chrono::system_clock::from_time_t(std::mktime(&endDate));
		 auto duration = std::chrono::duration_cast<std::chrono::duration<int, std::ratio<86400>>>(end - start);


		//Calculate total days
		int totalDays = duration.count();

		//Implement your cost calculation logic based on total days 
		//For example, a simple cost of $X per day
		const double costPerDay = 50.0;
		double totalCost = totalDays * costPerDay;


	//Perform the booking
	std::string insertBookingSQL = "INSERT INTO RentalBooking (CustomerID, CarID, StartDate, EndDate, Status) VALUES (?, ?, ?, ?, 'Pending');";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, insertBookingSQL.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, userID);
		sqlite3_bind_int(stmt, 2, carID);
		sqlite3_bind_text(stmt, 3, startDateStr.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, endDateStr.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			std::cout << "-----------------------------------------------------" << std::endl;
			std::cout << "Booking successful. Your request is pending approval.\n";
			std::cout << "-----------------------------------------------------" << std::endl;
			std::cout << "Total Days: " << totalDays << "\n";
			std::cout << "-----------------------------------------------------" << std::endl;
			std::cout << "Total Cost: " << totalCost << "\n";
			std::cout << "-----------------------------------------------------" << std::endl;
		}
		else {
			std::cerr << "Booking failed. Please try again.\n";
		}
		sqlite3_finalize(stmt);
	}
	else {
		std::cerr << "Failed to prepare the booking statement.\n";
	}
	std::cout << "==========================================\n";
}

void UserInterface::bookCar(const Car& selectedCar) {
	bookCarHeader();
	std::string startDateStr, endDateStr;

	// Display details of the selected car
	std::cout << "Selected Car Details:\n"
		<< "CarID: " << selectedCar.carID << "\n"
		<< "Make: " << selectedCar.make << "\n"
		<< "Model: " << selectedCar.model << "\n"
		<< "Year: " << selectedCar.year << "\n"
		<< "Mileage: " << selectedCar.mileage << "\n"
		<< "Availability: " << (selectedCar.isAvailable ? "Yes" : "No") << "\n\n";

	std::tm startDate = {};
	std::tm endDate = {};

	// Functions to get valid year, month, and day
	auto getValidYear = [](const std::string& prompt) -> int {
		int year;
		do {
			std::cout << prompt;
			std::cin >> year;
		} while (year < 2024 || year > 2030);
		return year;
		};

	auto getValidMonth = [](const std::string& prompt) -> int {
		int month;
		do {
			std::cout << prompt;
			std::cin >> month;
		} while (month < 1 || month > 12);
		return month;
		};

	auto getValidDay = [](const std::string& prompt, int maxDay) -> int {
		int day;
		do {
			std::cout << prompt;
			std::cin >> day;
		} while (day < 1 || day > maxDay);
		return day;
		};

	// Get the start date components
	startDate.tm_year = getValidYear("Enter the start year (2024-2030): ") - 1900;
	startDate.tm_mon = getValidMonth("Enter the start month (1-12): ") - 1; // Month is 0-based
	startDate.tm_mday = getValidDay("Enter the start day (1-31): ", 31);

	// Get the end date components
	endDate.tm_year = getValidYear("Enter the end year (2024-2030): ") - 1900;
	endDate.tm_mon = getValidMonth("Enter the end month (1-12): ") - 1; // Month is 0-based
	endDate.tm_mday = getValidDay("Enter the end day (1-31): ", 31);

	// Convert tm structures to strings
	std::ostringstream startDateStreamStr, endDateStreamStr;
	startDateStreamStr << std::put_time(&startDate, "%Y-%m-%d");
	endDateStreamStr << std::put_time(&endDate, "%Y-%m-%d");
	startDateStr = startDateStreamStr.str();
	endDateStr = endDateStreamStr.str();

	// Calculate the duration between start and end dates
	auto start = std::chrono::system_clock::from_time_t(std::mktime(&startDate));
	auto end = std::chrono::system_clock::from_time_t(std::mktime(&endDate));
	auto duration = std::chrono::duration_cast<std::chrono::duration<int, std::ratio<86400>>>(end - start);

	// Calculate total days and cost
	int totalDays = duration.count();
	const double costPerDay = 50.0;
	double totalCost = totalDays * costPerDay;

	// Perform the booking
	std::string insertBookingSQL = "INSERT INTO RentalBooking (CustomerID, CarID, StartDate, EndDate, Status) VALUES (?, ?, ?, ?, 'Pending');";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, insertBookingSQL.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, userID);
		sqlite3_bind_int(stmt, 2, selectedCar.carID);
		sqlite3_bind_text(stmt, 3, startDateStr.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, endDateStr.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			// Display booking details
			std::cout << "-----------------------------------------------------" << std::endl;
			std::cout << "Booking successful. Your request is pending approval.\n";
			std::cout << "-----------------------------------------------------" << std::endl;
			std::cout << "Total Days: " << totalDays << "\n";
			std::cout << "-----------------------------------------------------" << std::endl;
			std::cout << "Total Cost: " << totalCost << "\n";
			std::cout << "-----------------------------------------------------" << std::endl;
		}
		else {
			std::cerr << "Booking failed. Please try again.\n";
		}
		sqlite3_finalize(stmt);
	}
	else {
		std::cerr << "Failed to prepare the booking statement.\n";
	}
	std::cout << "==========================================\n";
}


void UserInterface::viewUserBookingsHeader() {
	std::cout << "==========================================\n";
	std::cout << "           View User Bookings\n";
	std::cout << "==========================================\n";
}

void UserInterface::viewUserBookings() {
	int choice;
	std::string statusFilter;

	viewUserBookingsHeader();

	std::cout << "Select booking type:\n";
	std::cout << "1. All Bookings\n"
		<< "2. Pending Bookings\n"
		<< "3. Confirmed Bookings\n"
		<< "4. Rejected Bookings\n"
		<< "5. Cancelled Bookings\n";
	std::cout << "Enter your choice: ";
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
	case 5:
		statusFilter = "Cancelled";
		break;
	default:
		std::cout << "Invalid choice.\n";
		return;
	}

	//function that retrieves and shows the bookings with the specified filter
	retrieveAndShowBookings(statusFilter);

	std::cout << "==========================================\n";
}

void UserInterface::retrieveAndShowBookings(const std::string& statusFilter) {
	std::string sql = "SELECT BookingID, Car.Make, Car.Model, Car.Year, StartDate, EndDate, Status FROM RentalBooking JOIN Car ON RentalBooking.CarID = Car.CarID WHERE CustomerID = " + std::to_string(userID);
	if (!statusFilter.empty()) {
		sql += " AND Status = '" + statusFilter + "'";
	}
	sql += ";";
	char* errMsg = nullptr;

	std::cout << "------------User-Booking---------" << std::endl;

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		//Fetch the first row
		int result = sqlite3_step(stmt);
		//Check if at least one row is found
		if (result == SQLITE_ROW) {
			do {
				//Proccess the current row
				std::cout << "BookingID: " << sqlite3_column_int(stmt, 0) << "\n"
					<< "Car: " << sqlite3_column_text(stmt, 1) << " " << sqlite3_column_text(stmt, 2) << " " << sqlite3_column_int(stmt, 3) << "\n"
					<< "StartDate: " << sqlite3_column_text(stmt, 4) << "\n"
					<< "EndDate: " << sqlite3_column_text(stmt, 5) << "\n"
					<< "Status: " << sqlite3_column_text(stmt, 6) << "\n"
					<< "----------------------------\n";
			} while (sqlite3_step(stmt) == SQLITE_ROW);
		}
		else {
			//No rows found
			std::cout << "No bookings found!\n";
		}
		//Finalize the statement to release resources
		sqlite3_finalize(stmt);
	}
	else {
		//Failed to prepare the SQL statement
		std::cerr << "Failed to prepare the SQL statement. \n";
	}

	//Footer for the booking section
	std::cout << "--------------------------------------" << std::endl;

}


//Helper function to check if there are pending bookings
bool UserInterface::hasPendingBookings() {
	std::string sql = "SELECT COUNT(*) FROM RentalBooking WHERE CustomerID = " + std::to_string(userID) + " AND Status = 'Pending';";
	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			int count = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
			return count > 0; //Return true if there are pending bookings
		}
	}

	if (stmt != nullptr) {
		sqlite3_finalize(stmt);
	}

	return false; //Return false in case of any error
}

void UserInterface::cancelBooking() {
	// Display user's pending bookings first
	retrieveAndShowBookings("Pending");

	//Check if there are no pending bookings
	if (!hasPendingBookings()) {
		std::cout << "No pending bookings available for cancellation.\n";
		std::cout << "--------------------------------------------------\n";
		return;
	}

	int bookingID;
	std::cout << "Enter the BookingID you want to cancel: ";
	std::cin >> bookingID;

	// Validate if the bookingID is valid (belongs to the user and is pending)
	std::string validateBookingSQL = "SELECT * FROM RentalBooking WHERE BookingID = ? AND CustomerID = ? AND Status = 'Pending';";
	sqlite3_stmt* validateStmt = nullptr;

	if (sqlite3_prepare_v2(db, validateBookingSQL.c_str(), -1, &validateStmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(validateStmt, 1, bookingID);
		sqlite3_bind_int(validateStmt, 2, userID);

		if (sqlite3_step(validateStmt) == SQLITE_ROW) {
			// BookingID is valid
			sqlite3_finalize(validateStmt);
			validateStmt = nullptr; // Reset the pointer to nullptr after finalization

			// Now, proceed with the cancellation
			std::string cancelBookingSQL = "UPDATE RentalBooking SET Status = 'Cancelled' WHERE BookingID = ?;";
			sqlite3_stmt* cancelStmt = nullptr;

			if (sqlite3_prepare_v2(db, cancelBookingSQL.c_str(), -1, &cancelStmt, nullptr) == SQLITE_OK) {
				sqlite3_bind_int(cancelStmt, 1, bookingID);

				if (sqlite3_step(cancelStmt) == SQLITE_DONE) {
					std::cout << "Booking cancelled successfully.\n";
				}
				else {
					std::cerr << "Failed to cancel booking. Please try again.\n";
				}

				sqlite3_finalize(cancelStmt);
			}
			else {
				std::cerr << "Failed to prepare the cancel booking statement.\n";
			}
		}
		else {
			// BookingID is invalid
			std::cerr << "Invalid BookingID. Please choose a valid pending booking to cancel.\n";
		}

		if (validateStmt != nullptr) {
			sqlite3_finalize(validateStmt);
		}
	}
	else {
		std::cerr << "Failed to prepare the validate booking statement.\n";
	}
}

void UserInterface::displayCars(const std::vector<Car>& cars) {
	if (cars.empty()) {
		std::cout << "No cars found matching the criteria.\n";
	}
	else {
		std::cout << "--------------------------------------------------\n";
		std::cout << std::left << std::setw(7) << "Index" << std::setw(10) << "CarID" << std::setw(15) << "Make"
			<< std::setw(15) << "Model" << std::setw(10) << "Year" << std::setw(15) << "Mileage" << "Availability\n";
		std::cout << "--------------------------------------------------\n";
		for (size_t i = 0; i < cars.size(); ++i) {
			const auto& car = cars[i];
			std::cout << std::left << std::setw(7) << i << std::setw(10) << car.carID << std::setw(15) << car.make
				<< std::setw(15) << car.model << std::setw(10) << car.year << std::setw(15) << car.mileage
				<< (car.isAvailable ? "Yes" : "No") << "\n";
		}
		std::cout << "-------------------------------------------------\n";
	}
}

void UserInterface::searchCars() {
	std::string make, model;
	bool availableOnly;

	//Get search criteria from the user
	std::cout << "+---------------------------------------------+\n";
	std::cout << "|               Search Cars                   |\n";
	std::cout << "+---------------------------------------------+\n";
	std::cout << "| Enter make (or leave empty):                |\n";
	std::cin.ignore();
	std::cout << "| ";
	std::getline(std::cin, make);
	std::cout << "|                                             |\n";
	std::cout << "| Enter model (or leave empty):               |\n";
	std::cout << "| ";
	std::getline(std::cin, model);
	std::cout << "|                                             |\n";
	std::cout << "| Search for available cars only? (1 for Yes, |\n";
	std::cout << "| 0 for No):                                  |\n";
	std::cout << "| ";
	std::cin >> availableOnly;
	std::cout << "+---------------------------------------------+\n";

	//Call the CarManager function to search for cars
	std::vector<Car> searchResults = carManager.searchCars(make, model, availableOnly);

	// Display the search results
	displayCars(searchResults);

	//Book a car if there are search results
	if (!searchResults.empty()) {
		std::cout << "+------------------------------------------------+\n";
		std::cout << "| Do you want to book a car? (1 for Yes,         |\n";
		std::cout << "| 0 for No):                                     |\n";
		std::cout << "| ";
		int choice;
		std::cin >> choice;

		if (choice == 1) {
			int carIndex;
			std::cout << "| Enter the index of the car you want to book:|\n";
			std::cout << "| ";
			std::cin >> carIndex;

			if (carIndex >= 0 && carIndex < searchResults.size()) {
				//Book the selected car
				bookCar(searchResults[carIndex]);
			}
			else {
				std::cerr << "| Invalid car index. Please enter a valid    |\n";
				std::cerr << "| index.                                     |\n";
			}
		}
		else {
			std::cout << "| Booking canceled.                          |\n";
		}
	}
	else {
		std::cout << "| No cars found matching the criteria.       |\n";
	}
	std::cout << "+--------------------------------------------+\n";
}

