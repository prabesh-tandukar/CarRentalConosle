#pragma once
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <sqlite3.h>

class UserInterface
{
private: 
	sqlite3* db;
public:
	UserInterface(sqlite3* db);
	void showMenu();
	void viewAvailableCars();
	void bookCar();
	void viewOwnBookings();
	void manageBooking();
	//Other user-related methods
};

#endif //USER_INTERFACE_H

