#pragma once
#ifndef ADMIN_INTERFACE_H
#define ADMIN_INTERFACE_H

#include <sqlite3.h>

class AdminInterface
{
private:
	sqlite3* db;
public:
	AdminInterface(sqlite3* db);
	void showMenu();
	void addNewCar();
	void updateCarDetails();
	void manageCars();
	void viewMangeBookings();
	void manageUserAccounts();
};

#endif //ADMIN_INTERFACE_H
