#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>
#include <math.h>
#include <float.h>

struct Warehouse
{
    int x;
    int y;
};

class Customer {
    public:
        Customer(int initial_id, double initial_demand, double initial_x, double initial_y) {
            id = initial_id;
            demand = initial_demand;
            x = initial_x;
            y = initial_y;
            vehicle_id = -1;
        }

        int Id() const {
            return id;
        }

        double Demand() const {
            return demand;
        }

        std::pair<double, double> Coords() const {
            return std::make_pair(x, y);
        }

        int GetVehicleId() const {
            return vehicle_id;
        }

        void AssignToVehicle(int new_vehicle_id) {
            vehicle_id = new_vehicle_id;
        }

        void UnassignVehicle() {
            vehicle_id = -1;
        }

        bool CheckAssigned() const {
            return vehicle_id != -1;
        }

        double GetDistTo(double to_x, double to_y) const {
            return sqrt(pow(x - to_x, 2) + pow(y - to_y, 2));
        }

    private:
        int id;
        double demand;
        double x, y;
        int vehicle_id;
};

#endif