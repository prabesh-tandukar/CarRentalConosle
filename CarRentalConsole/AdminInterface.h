#pragma once
#ifndef ADMIN_INTERFACE_H
#define ADMIN_INTERFACE_H

#include <sqlite3.h>

class AdminInterface
{
public:
	AdminInterface(sqlite3* db);
	void showMenu();
};

#endif //ADMIN_INTERFACE_H
