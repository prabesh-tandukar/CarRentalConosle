#include "CarManager.h"
#include <iostream>

CarManager::CarManager(sqlite3* db) : db(db){}

std::vector<Car> CarManager::searchCars(const std::string& make, const std::string& model, bool availableOnly) {
    // Construct SQL query based on search criteria
    std::string sql = "SELECT * FROM Car WHERE 1=1";
    if (!make.empty()) {
        sql += " AND Make = '" + make + "'";
    }
    if (!model.empty()) {
        sql += " AND Model = '" + model + "'";
    }
    if (availableOnly) {
        sql += " AND IsAvailable = 1";
    }
    sql += ";";

    // Execute the query and retrieve the matching cars
    std::vector<Car> result;
    char* errMsg = nullptr;

    if (sqlite3_exec(db, sql.c_str(), carCallback, &result, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    return result;
}


int CarManager::carCallback(void* data, int argc, char** argv, char** azColName) {
    std::vector<Car>* cars = static_cast<std::vector<Car>*>(data);

    Car car;
    for (int i = 0; i < argc; i++) {
        std::string colName(azColName[i]);
        std::string colValue(argv[i] ? argv[i] : "");

        if (colName == "CarID") {
            car.carID = std::stoi(colValue);
        }
        else if (colName == "Make") {
            car.make = colValue;
        }
        else if (colName == "Model") {
            car.model = colValue;
        }
        else if (colName == "Year") {
            car.year = std::stoi(colValue);
        }
        else if (colName == "Mileage") {
            car.mileage = std::stod(colValue);
        }
        else if (colName == "IsAvailable") {
            car.isAvailable = (colValue == "1");
        }
        // Add more conditions based on your Car class attributes
    }

    cars->push_back(car);

    return 0;  // Return value depends on your implementation
}
