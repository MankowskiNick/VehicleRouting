#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include "customer.h"

class Vehicle {
    public:
        static double inline VEHICLE_CAPACITY = 0.0f;
        
        Vehicle(int initial_id) {
            id = initial_id;
            customer_ids = std::vector<int>();
            capacity_left = VEHICLE_CAPACITY;
        }

        Vehicle() {}

        int Id() const {
            return id;
        }

        bool AssignCustomer(Customer& new_cust) {
            if (new_cust.Demand() <= capacity_left) {
                new_cust.AssignToVehicle(Id());
                customer_ids.push_back(new_cust.Id());
                capacity_left -= new_cust.Demand();
                return true;
            }
            return false;
        }

        bool RemoveCustomer(Customer& remove_cust) {        
            // Get the id of the customer to remove
            int remove_cust_id = remove_cust.Id();

            // Default item index, since -1 is impossible as a vect position this will indicate an error
            int item_index = -1;
            int list_size = customer_ids.size();
            for (int i = 0; i < list_size && list_size > 0; i++) {

                // If we've found the item we need to remove it by shifting all of 
                // the elements to the right and then decrease vector size by 1.
                if (customer_ids[i] == remove_cust_id && item_index == -1) {
                    item_index = i;
                    for (int j = i; j < list_size; j++) {
                        customer_ids[j] = customer_ids[j + 1];
                    }
                    list_size--;
                    customer_ids.resize(list_size);
                }
            }
            
            // If item_index is still -1, the element wasn't found so we should return an error.
            if (item_index != -1) {
                remove_cust.UnassignVehicle();
                capacity_left += remove_cust.Demand();
                return true;
            }
            return false;
        }

        std::vector<int> GetCustomerIds() const {
            return std::vector<int>(customer_ids);
        }

        double Capacity() const {
            return capacity_left;
        }

    private:
        int id;
        double capacity_left;
        std::vector<int> customer_ids;
};

#endif 