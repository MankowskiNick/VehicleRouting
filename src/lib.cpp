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
#define START_TEMP_SCALAR 50
#define TEMP_MIN 1

// Get the index of the minimum element
template<typename T>
int GetMinIndex(std::vector<T>& vect) {
    int index = 0;
    T min_result = vect[0];

    for (int i = 0; i < vect.size(); i++) {
        if (vect[i] < min_result) {
            index = i;
            min_result = vect[i];
        }
    }
    return index;
}
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
    double warehouse_x, warehouse_y;
    fin >> warehouse_x; // Skip over the "demand" of the warehouse, just use warehouse_x as a placeholder
    fin >> warehouse_x;
    fin >> warehouse_y;

    warehouse.x = warehouse_x;
    warehouse.y = warehouse_y;

    // Map customer data
    for (int i = 1; i < num_customers; i++) {
        double cust_x, cust_y, cust_dem;

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

double ScoreSolution(const std::vector<Customer>& customers, const std::vector<Vehicle>& vehicles, const struct Warehouse& warehouse) {
    double result = 0.0f;
    for (int i = 0; i < vehicles.size(); i++) {
        std::vector<int> customer_ids = vehicles[i].GetCustomerIds();
        if (customer_ids.size() > 0) {
            double cur_result = DistToWarehouse(customers[customer_ids[0]], warehouse);
            for (int j = 0; j < customer_ids.size() - 1; j++) {
                int cust1_id = customer_ids[j];
                int cust2_id = customer_ids[j + 1];
                cur_result += GetDist(customers[cust1_id], customers[cust2_id]);
            }
            int last_cust_id = customer_ids[customer_ids.size() - 1];
            cur_result += DistToWarehouse(customers[last_cust_id], warehouse);
            result += cur_result;
        }
    }
    return result;
}

double GetGreedySolution(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse) {
    for (int i = 0; i < customers.size(); i++) {
        int vehicle_id = rand() % vehicles.size();
        for (int j = 0; j < vehicles.size(); j++) {
            if (vehicles[j].AssignCustomer(customers[i]))
                break;
        }
    }
    return ScoreSolution(customers, vehicles, warehouse);
}

void MoveCustomer(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, int cust_index) {
    
    int old_vehicle_id = customers[cust_index].GetVehicleId();
    int new_vehicle_id = old_vehicle_id;

    while (new_vehicle_id == old_vehicle_id)
        new_vehicle_id = rand() % vehicles.size();

    if (old_vehicle_id != -1)
        vehicles[old_vehicle_id].RemoveCustomer(customers[cust_index]);
    if (vehicles[new_vehicle_id].AssignCustomer(customers[cust_index])) {
        return;
    } else {
        while(!vehicles[new_vehicle_id].AssignCustomer(customers[cust_index])) {
            int remove_id = vehicles[new_vehicle_id].GetCustomerIds()[0];
            vehicles[new_vehicle_id].RemoveCustomer(customers[remove_id]);
            MoveCustomer(customers, vehicles, remove_id);
        }
        return;
    }
}

void MoveCustomers(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles) {
    int cust_id = rand() % customers.size();
    MoveCustomer(customers, vehicles, cust_id);
}

// Perform simulated annealing
double Anneal(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse, int num_changes) { 
    double current_best = ScoreSolution(customers, vehicles, warehouse);
    double pre_annealing_check = ScoreSolution(customers, vehicles, warehouse);

    // Simulated Annealing
    // Initialize temperature
    double initial_temp = (START_TEMP_SCALAR * START_TEMP_SCALAR) / double(customers.size());
    double temp = initial_temp;
    
    // Start simulated annealing
    int iteration = 1;
    while (temp > TEMP_MIN) {
        
        std::vector<Vehicle> annealing_vehicles = std::vector<Vehicle>(vehicles);
        std::vector<Customer> annealing_customers = std::vector<Customer>(customers);

        // Make a random change
        // Relocate the specified number of cities in the order(in the vector, obviously)
        for (int i = 0; i < num_changes; i++) {
            MoveCustomers(annealing_customers, annealing_vehicles);
        }

        double check = ScoreSolution(annealing_customers, annealing_vehicles, warehouse);

        double take_probability = exp(-1 * abs(check - current_best) / (temp + 1));
        double random = double(rand()) / double(RAND_MAX);
        if (check < current_best || random < take_probability) {
            current_best = check;
            vehicles = std::vector<Vehicle>(annealing_vehicles);
            customers = std::vector<Customer>(annealing_customers);
        }
        temp = initial_temp / double(iteration);
        iteration++;

    }
    return current_best;
}

double RouteVehicles(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse) {
    srand(time(NULL));

    double greedy_result = GetGreedySolution(customers, vehicles, warehouse);
    
    std::vector< std::vector<Vehicle> > vehicle_annealing_results;
    std::vector< std::vector<Customer> > customer_annealing_results;
    std::vector<double> annealing_results;

    vehicle_annealing_results.push_back(vehicles);
    customer_annealing_results.push_back(customers);
    annealing_results.push_back(greedy_result);

    for (int i = 1; i < 5; i++) {
        std::vector<Vehicle> cur_vehicles = std::vector<Vehicle>(vehicles);
        std::vector<Customer> cur_customers = std::vector<Customer>(customers);

        double cur_result = Anneal(cur_customers, cur_vehicles, warehouse, i);

        annealing_results.push_back(cur_result);
        customer_annealing_results.push_back(cur_customers);
        vehicle_annealing_results.push_back(cur_vehicles);
    }

    int min_index = GetMinIndex<double>(annealing_results);
    vehicles = std::vector<Vehicle>(vehicle_annealing_results[min_index]);
    customers = std::vector<Customer>(customer_annealing_results[min_index]);
    return ScoreSolution(customers, vehicles, warehouse);
}