#pragma once
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <sqlite3.h>
#include <string>
#include <iostream>
#include "CarManager.h"

class UserInterface
{
public:
	UserInterface(sqlite3* db, int userID);
	bool showMenu();

	

private: 
	sqlite3* db;
	int userID;
	CarManager carManager;

	void viewAvailableCars();
	//void viewCarDetails();
	void bookCar();
	void bookCar(const Car& selectedCar);
	bool validateCarID(int carID);
	void viewUserBookings();
	void retrieveAndShowBookings(const std::string& statusFilter);
	void cancelBooking();
	bool hasPendingBookings();
	void searchCars();
	void displayCars(const std::vector<Car>& cars);
	//Other user-related methods
};

#endif //USER_INTERFACE_H

