#include "PasswordManager.h"
#include <iostream>
#include <string>
#include <conio.h>


unsigned long PasswordManager::djb2Hash(const std::string& str) {
		unsigned long hash = 5381;

		for (char c : str) {
			hash = (hash << 5) + hash + c; //hash * 33 + c
		}

		return hash;
	}

	 std::string PasswordManager::promptForPassword() {
		std::string password;
		char ch;
		std::cout << "Enter your password: ";

		//Hide input for password
		while ((ch = _getch()) != 13) { //13 is the ASCII code for Enter
			if (ch == '\b') { //Handle backspace
				if (!password.empty()) {
					std::cout << "\b \b";
					password.pop_back();
				}
			}
			else {
				std::cout << '*';
				password.push_back(ch);
			}

		}
		std::cout << std::endl; //Move to the next line after password
		return password;
	}

