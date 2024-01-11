#pragma once
#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

#include <sqlite3.h>
#include <string>


class BookingManager
{
private:
	sqlite3* db;
	static int callback(void* NotUsed, int argc, char** argv, char** azColName);
public:
	BookingManager(sqlite3* db);
	void showMenu();
	void viewAllBookings();
	void updateBooking();
	void approveBooking();
	void rejectBooking();
	void cancelBooking();

};

#endif
