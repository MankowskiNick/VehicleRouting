#ifndef LIB_H
#define LIB_H

#include <iostream>
#include <vector>
#include <fstream>

#include "vehicle.h"
#include "customer.h"

void InputMapper(std::ifstream& fin, std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse);
double RouteVehicles(std::vector<Customer>& customers, std::vector<Vehicle>& vehicles, struct Warehouse& warehouse);

#endif