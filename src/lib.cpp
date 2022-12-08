#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <functional>

#include "quicksort.h"
#include "customer.h"
#include "vehicle.h"

#define GREEDY_TIME_MAX 6
#define START_TEMP_SCALAR 10000
#define TEMP_MIN 1
// Insert item into a vector at a given position
template<typename T>
bool InsertAtPosition(std::vector<T>& vect, T& add_item, int index) {
    vect.resize(vect.size() + 1);
    if (index >= vect.size() || index < 0) throw std::out_of_range ("Out of Range Exception: Insert index is out of range of vector.");
    for (int i = vect.size() - 2; i >= index; i--) {
        vect[i + 1] = vect[i];
    }
    vect[index] = add_item;
    return true;
}

template<typename T>
T SumVect(const std::vector<T>& v) {
    T sum = T();
    for (int i = 0; i < v.size(); i++) {
        sum += v[i];
    }
    return sum;
}

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

// Remove the given item from the given vector
template<typename T>
int RemoveFromVector(std::vector<T>& vect, T remove_item) {
    int item_index = -1;
    int list_size = vect.size();
    for (int i = 0; i < list_size; i++) {
        if (vect[i] == remove_item && item_index == -1) {
            item_index = i;
            list_size--;
            for (int j = i; j < vect.size(); j++) {
                vect[j] = vect[j + 1];
            }
            vect.resize(list_size);
        }
    }
    return item_index;
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

// Relocate a random customer in a given route
void SpliceRandomCustomer(std::vector<Customer>& customers) {

    Customer rand_cust;

    // Pick a random customer to remove
    int remove_index = rand() % (customers.size() - 1);
    rand_cust = customers[remove_index];

    // Remove that customer
    RemoveFromVector(customers, rand_cust);

    // Place that customer in a new spot somewhere after where it was removed
    int constraint = customers.size() - remove_index + 1;
    int random = rand() % constraint;
    int new_index = random + remove_index;
    InsertAtPosition(customers, rand_cust, new_index);
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

double ScoreRoute(const std::vector<Customer>& customers, const struct Warehouse& warehouse) {
    double result = DistToWarehouse(customers[0], warehouse);
    for (int i = 0; i < customers.size() - 1; i++) {
        result += GetDist(customers[i], customers[i + 1]);
    }
    result += DistToWarehouse(customers[customers.size() - 1], warehouse);
    return result;
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

void MoveCustomer_Helper(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, int cust_index) {
    
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
            std::vector<int> customer_ids =  vehicles[new_vehicle_id].GetCustomerIds();
            //std::random_shuffle(customer_ids.begin(), customer_ids.end());
            int rand_index = rand() % customer_ids.size();
            int remove_id = customer_ids[rand_index];
            vehicles[new_vehicle_id].RemoveCustomer(customers[remove_id]);
            MoveCustomer_Helper(customers, vehicles, remove_id);
        }
        return;
    }
}

void MoveCustomer(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles) {
    int cust_id = rand() % customers.size();
    MoveCustomer_Helper(customers, vehicles, cust_id);
}

double GetGreedySolution(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse) {
    /*
    if (GREEDY_SOLUTION == 0) {
        for (int i = 0; i < customers.size(); i++) {
            for (int j = 0; j < vehicles.size(); j++) {
                if (vehicles[j].AssignCustomer(customers[i]))
                    break;
            }
        }
    }
    else if (GREEDY_SOLUTION == 1) {
        for (int i = 0; i < vehicles.size(); i++) {
            for (int j = 0; j < customers.size(); j++) {
                if (vehicles[i].AssignCustomer(customers[j]))
                    break;
            }
        }
    }
    */

   

   time_t start_time = time(NULL);
    for (int i = 0; i < customers.size() && time(NULL) - start_time < GREEDY_TIME_MAX; i++) {
        for (int j = 0; j < vehicles.size() && time(NULL) - start_time < GREEDY_TIME_MAX; j++) {
            if (vehicles[j].AssignCustomer(customers[i]))
                break;
        }
    }
    if (time(NULL) - start_time > GREEDY_TIME_MAX) {
        start_time = time(NULL);
        for (int i = 0; i < vehicles.size(); i++) {
            for (int j = 0; j < customers.size(); j++) {
                if (vehicles[i].AssignCustomer(customers[j]))
                    break;
            }
        }
    }
    /*
    if (time(NULL) - start_time > GREEDY_TIME_MAX) {
        std::vector<int> available_ids;
        for (int i = 0; i < customers.size(); i++ ) {
            available_ids.push_back( customers[i].Id() );
        }

        std::vector< std::vector<int> > routes;
        int cur_vehicle_id = 0;
        for (int i = 0; i < vehicles.size(); i++) {
            std::vector<int> ids;
            std::vector<double> distances_to_ids;
            for (int j = 1; j < available_ids.size(); j++) {
                int cust1_id = available_ids[0];
                int cust2_id = available_ids[j];
                double cur_dist = GetDist(customers[cust1_id], customers[cust2_id]);
                ids.push_back(j);
                distances_to_ids.push_back(cur_dist);
            }

            QuickSort2<double, int>::Sort(distances_to_ids, ids);

            vehicles[i].AssignCustomer(customers[available_ids[0]]);
            for (int j = 0; j < ids.size(); j++) {
                int cust_id = ids[j];
                if (vehicles[i].AssignCustomer(customers[cust_id])) {
                    RemoveFromVector(available_ids, cust_id);
                }
            }
            RemoveFromVector(available_ids, available_ids[0]);
        }

        for (int i = 0; i < available_ids.size(); i++) {
            int vehicle_id = rand() % vehicles.size();
            MoveCustomer_Helper(customers, vehicles, available_ids[i]);
        }
    }
    */

    return ScoreSolution(customers, vehicles, warehouse);
}

// Get the 'greedy solution' for each vehicle route
double GetGreedyRoute(std::vector<Customer>& cust_list, struct Warehouse& warehouse, int start_node = 0) {
    double result = 0.0f;
    if (cust_list.size() > 0) {
        std::vector<Customer> ref_list(cust_list);
        cust_list = std::vector<Customer>();
        cust_list.push_back(ref_list[start_node]);
        RemoveFromVector<Customer>(ref_list, ref_list[0]);

        result = DistToWarehouse(cust_list[0], warehouse);

        int num_iterations = ref_list.size();
        for (int i = 0; i < num_iterations; i++) {
            std::vector<double> possible_dist;
            std::vector<int> possible_index;

            // Try all moves, take the cheapest
            for (int j = 0; j < ref_list.size(); j++) {
                if (cust_list[cust_list.size() - 1].Id() != ref_list[j].Id()) {
                    possible_dist.push_back(GetDist(cust_list[cust_list.size() - 1], ref_list[j]));
                    possible_index.push_back(j);
                }
            }
            int min_index = GetMinIndex(possible_dist);
            int cust_index = possible_index[min_index];
            double cur_result = possible_dist[min_index];

            cust_list.push_back(ref_list[cust_index]);
            RemoveFromVector<Customer>(ref_list, ref_list[cust_index]);

            result += cur_result;
        }
        result += DistToWarehouse(cust_list[cust_list.size() - 1], warehouse);
    }
    return result;

}

// Perform simulated annealing
//double Anneal(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse, std::function<void(std::vector<Customer>, std::vector<Vehicle>)> change_func, int num_changes) { 
double Anneal(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse, void (*change_func)(std::vector<Customer>&, std::vector<Vehicle>&), int num_changes) { 
    double current_best = ScoreSolution(customers, vehicles, warehouse);
    double pre_annealing_check = ScoreSolution(customers, vehicles, warehouse);

    // Simulated Annealing
    // Initialize temperature
    double initial_temp = (START_TEMP_SCALAR) / double(customers.size());
    double temp = initial_temp;
    
    // Start simulated annealing
    int iteration = 1;
    while (temp > TEMP_MIN) {
        
        std::vector<Vehicle> annealing_vehicles = std::vector<Vehicle>(vehicles);
        std::vector<Customer> annealing_customers = std::vector<Customer>(customers);

        // Make a random change
        // Relocate the specified number of cities in the order(in the vector, obviously)
        for (int i = 0; i < num_changes; i++) {
            change_func(annealing_customers, annealing_vehicles);
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

double AnnealRouteOrder(std::vector<Customer>& customers, struct Warehouse& warehouse, void (*change_func)(std::vector<Customer>&), int num_changes = 1) {

    double current_best = ScoreRoute(customers, warehouse);

    // Simulated Annealing
    // Initialize temperature
    double initial_temp = (START_TEMP_SCALAR * START_TEMP_SCALAR) / double(customers.size());
    double temp = double(initial_temp);
    
    // Start simulated annealing
    int iteration = 1;
    while (temp > TEMP_MIN) {
        
        std::vector<Customer> annealing_list = std::vector<Customer>(customers);

        // Make a random change
        // Relocate the specified number of cities in the order(in the vector, obviously)
        for (int i = 0; i < num_changes; i++) {
            change_func(annealing_list);
        }

        double check = ScoreRoute(annealing_list, warehouse);

        double take_probability = exp(-1 * abs(check - current_best) / (temp + 1));
        double random = double(rand()) / double(RAND_MAX);
        if (check < current_best || (random < take_probability && abs(check - current_best) > 0.01 )) {
            current_best = check;
            customers = std::vector<Customer>(annealing_list);
        }
        temp = initial_temp / double(iteration);
        iteration++;

    }
    return current_best;
}

double GetBestRoute(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse, int cur_vehicle) {
    std::vector<int> customer_ids = vehicles[cur_vehicle].GetCustomerIds();
    if (customer_ids.size() > 1) {
        std::vector<Customer> cur_cust_list;
        for (int i = 0; i < customer_ids.size(); i++) {
            int cur_cust_id = customer_ids[i];
            cur_cust_list.push_back(customers[cur_cust_id]);
        }

        double greedy_solution = GetGreedyRoute(cur_cust_list, warehouse);

        // anneal?
        AnnealRouteOrder(cur_cust_list, warehouse, &SpliceRandomCustomer);

        customer_ids = std::vector<int>();
        for (int i = 0; i < cur_cust_list.size(); i++) {
            customer_ids.push_back(cur_cust_list[i].Id());
        }

        vehicles[cur_vehicle].customer_ids = std::vector<int>(customer_ids);
    }
    return ScoreRoute(customers, warehouse);
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

        double cur_result = Anneal(cur_customers, cur_vehicles, warehouse, &MoveCustomer, i);

        annealing_results.push_back(cur_result);
        customer_annealing_results.push_back(cur_customers);
        vehicle_annealing_results.push_back(cur_vehicles);
    }

    int min_index = GetMinIndex<double>(annealing_results);
    vehicles = std::vector<Vehicle>(vehicle_annealing_results[min_index]);
    customers = std::vector<Customer>(customer_annealing_results[min_index]);

    double pre_routing_check = ScoreSolution(customers, vehicles, warehouse); // temp

    // Optimize each vehicle route
    for (int i = 0; i < vehicles.size(); i++) {
        GetBestRoute(customers, vehicles, warehouse, i);
    }
    double post_routing_check = ScoreSolution(customers, vehicles, warehouse); // temp
    return ScoreSolution(customers, vehicles, warehouse);
}