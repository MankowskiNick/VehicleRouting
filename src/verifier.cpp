#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <cmath>

using namespace std;

struct Customer{
  double demand;
  double x_loc;
  double y_loc;
  int cust_num;
};

struct Vehicle{
  double cap_remaining;
  vector<int> route;
};
double Compute_Distance(double x1, double y1, double x2, double y2){
  return sqrt(pow(x1-x2,2) + pow(y1-y2,2));
}

double Compute_Customer_Distance(const vector<Customer>& cust_list, int cust_1, int cust_2){
  double x1 = cust_list[cust_1].x_loc;
  double x2 = cust_list[cust_2].x_loc;
  double y1 = cust_list[cust_1].y_loc;
  double y2 = cust_list[cust_2].y_loc;
  return Compute_Distance(x1,y1,x2,y2);
}

double Compute_Cost(const vector<Vehicle>& vehicle_list, const vector<Customer> &cust_list){
  double total = 0;
  for(int i = 0; i < vehicle_list.size(); i++){
    int route_length = vehicle_list[i].route.size();
    for(int j = 0; j < route_length-1; j++){
      int first = vehicle_list[i].route[j];
      int second = vehicle_list[i].route[j+1];
      total = total + Compute_Customer_Distance(cust_list, first, second);
    }
    total = total + Compute_Customer_Distance(cust_list, vehicle_list[i].route[route_length-1], 0);
  }
  return total;
}

void Verify(string input_filename, string output_filename) {
  ifstream input, output;
  input.open(input_filename.data());
  output.open(output_filename.data());
  assert(input.is_open());
  assert(output.is_open());
  int n;
  input >> n;
  int v;
  input >> v;
  int capacity;
  input >> capacity;
  double warehouse_x;
  double warehouse_y;
  double temp;
  input >> temp; // the warehouse demand is always 0
  input >> warehouse_x;
  input >> warehouse_y;
  vector<Customer> cust_list(n);
  for(int i = 1; i < n; i++){
    input >> cust_list[i].demand;
    input >> cust_list[i].x_loc;
    input >> cust_list[i].y_loc;
    cust_list[i].cust_num = i;
  }
  double claimed_cost;
  output >> claimed_cost;
  cust_list[0].x_loc = warehouse_x;
  cust_list[0].y_loc = warehouse_y;
  cust_list[0].demand = 0;
  vector<Vehicle> vehicle_list(v);
  vector<bool> customers_seen(n, false);
  for(int i = 0; i < v; i++){
    vehicle_list[i].cap_remaining = capacity;
    int zero;
    output >> zero; // all routes start with 0
    if(zero != 0) // or, they'd better..
      cout << "ERROR: Vehicle " << i << " does not start with location 0!" << endl;
    vehicle_list[i].route.push_back(zero);
    int next;
    output >> next;
    while(next != 0){
      customers_seen[next] = true;
     
      vehicle_list[i].route.push_back(next);
      vehicle_list[i].cap_remaining -= cust_list[next].demand;
      if(vehicle_list[i].cap_remaining < 0)
	cout << "ERROR: Vehicle " << i << " over capacity!" << endl;
      output >> next;
    }
    vehicle_list[i].route.push_back(0);
  }
  for(int i = 1; i < customers_seen.size(); i++){
    if(!customers_seen[i])
      cout << "ERROR: No vehicle for customer " << i << endl;
  }
  double total_cost = Compute_Cost(vehicle_list, cust_list);
  cout << "Claimed cost: " << claimed_cost << endl;
  cout << "Actual cost: " << total_cost << endl;
}

int main(int argc, char* args[]){
    for (int i = 1; i < argc; i+=2) {    
      std::cout << "Checking file - " << string(args[i]) << "\n";
      Verify(string(args[i]), string(args[i + 1]));
      std::cout << "\n\n\n";
    }
}
