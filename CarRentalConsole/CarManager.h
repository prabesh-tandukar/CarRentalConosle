#ifndef CAR_MANAGER_H
#define CAR_MANAGER_H

#include <vector>
#include <string>
#include <sqlite3.h>
#include "Car.h" // Include the header file for the Car class

class CarManager {
public:
    CarManager(sqlite3* db);
    std::vector<Car> getAllCars();
    std::vector<Car> searchCars(const std::string& make, const std::string& model, bool availableOnly);

private:
    sqlite3* db;
    static int carCallback(void* data, int argc, char** argv, char** azColName);
};

#endif // CAR_MANAGER_H
