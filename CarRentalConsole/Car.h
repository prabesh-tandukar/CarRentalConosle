#ifndef CAR_H
#define CAR_H

#include <string>

class Car {
public:
    int carID;
    std::string make;
    std::string model;
    int year;
    int mileage;
    bool isAvailable;

    // Constructors
    Car();
    Car(int id, const std::string& make, const std::string& model, int year, int mileage, bool isAvailable);
};

#endif // CAR_H
