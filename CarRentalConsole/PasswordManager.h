#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <iostream>
#include <string>

class PasswordManager
{
public:
	//static std::string hashPassword(const std::string& password);

	static std::string promptForPassword();

	static unsigned long djb2Hash(const std::string& str);
};

#endif