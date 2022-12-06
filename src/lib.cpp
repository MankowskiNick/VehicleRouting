#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "quicksort.h"
#include "customer.h"
#include "vehicle.h"

#define TIME_MAX 60
#define START_TEMP_SCALAR 1000
#define TEMP_MIN 1

void InputMapper(std::ifstream& fin, std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse) {
    customers = std::vector<Customer>();
    vehicles = std::vector<Vehicle>();
    
    int num_customers, num_vehicles;
    double vehicle_capacity;

    fin >> num_customers;
    fin >> num_vehicles;
    fin >> vehicle_capacity;

    Vehicle().VEHICLE_CAPACITY = vehicle_capacity;

    // Map warehouse data
    int warehouse_x, warehouse_y;
    fin >> warehouse_x; // Skip over the "demand" of the warehouse, just use warehouse_x as a placeholder
    fin >> warehouse_x;
    fin >> warehouse_y;

    warehouse.x = warehouse_x;
    warehouse.y = warehouse_y;

    // Map customer data
    for (int i = 1; i < num_customers; i++) {
        int cust_x, cust_y;
        double cust_dem;

        fin >> cust_dem;
        fin >> cust_x;
        fin >> cust_y;

        customers.push_back( Customer(i - 1, cust_dem, cust_x, cust_y) ); // Using i might be a source of error, later on
    }

    // Initialize the proper amount of vehicles
    for (int i = 0; i < num_vehicles; i++) {
        vehicles.push_back( Vehicle(i) );
    }
}

double GetDistCoords(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

double GetDist(const Customer& cust1, const Customer& cust2) {
    std::pair<double, double> cust1_coords = cust1.Coords();
    std::pair<double, double> cust2_coords = cust2.Coords();
    return GetDistCoords(cust1_coords.first, cust1_coords.second, cust2_coords.first, cust2_coords.second);
}

double DistToWarehouse(const Customer& cust, const struct Warehouse& warehouse) {
    std::pair<double, double> cust_coords = cust.Coords();
    return GetDistCoords(cust_coords.first, cust_coords.second, warehouse.x, warehouse.y);
}

double ScoreSolution(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse) {
    double result = 0.0f;
    for (int i = 0; i < vehicles.size(); i++) {
        std::vector<int> customer_ids = vehicles[i].GetCustomerIds();
        double cur_result = DistToWarehouse(customers[customer_ids[0]], warehouse);
        for (int j = 0; j < customer_ids.size() - 1; i++) {
            int cust1_id = customer_ids[j];
            int cust2_id = customer_ids[j + 1];
            cur_result += GetDist(customers[cust1_id], customers[cust2_id]);
        }
        int last_cust_id = customer_ids[customer_ids.size() - 1];
        cur_result += DistToWarehouse(customers[customer_ids[last_cust_id]], warehouse);
        result += cur_result;
    }
    return result;
}

double GetGreedySolution(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse) {
    
    
    return ScoreSolution(customers, vehicles, warehouse);
}

// Perform simulated annealing
double Anneal() {
    return 0.0f;
}

double RouteVehicles() {
    return 0.0f;
}