#pragma once
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <sqlite3.h>
#include <string>
#include <iostream>


class UserInterface
{
public:
	UserInterface(sqlite3* db, int userID);
	bool showMenu();

	

private: 
	sqlite3* db;
	int userID;

	void viewAvailableCars();
	//void viewCarDetails();
	void bookCar();
	bool validateCarID(int carID);
	void viewUserBookings();
	void retrieveAndShowBookings(const std::string& statusFilter);
	void cancelBooking();
	bool hasPendingBookings();
	void searchCars();
	void displayCars();
	//Other user-related methods
};

#endif //USER_INTERFACE_H

