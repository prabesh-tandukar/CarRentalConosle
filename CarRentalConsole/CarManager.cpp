#include "CarManager.h"
#include <iostream>


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
