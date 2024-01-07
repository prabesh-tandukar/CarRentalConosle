#pragma once
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <sqlite3.h>

class UserInterface
{
public:
	UserInterface(sqlite3* db);
	void showMenu();
	//Other user-related methods
};

#endif //USER_INTERFACE_H

