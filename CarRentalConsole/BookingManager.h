#pragma once
#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

#include <sqlite3.h>
#include <string>


class BookingManager
{
public:
	BookingManager(sqlite3* db);
	void viewAllBookings();
	void updateBooking();
	void cancelBooking();
private: sqlite3* db;
};

#endif
