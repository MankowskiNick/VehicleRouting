// Nick Mankowski
// CS 490 - Fall 2022
// Prof. Sean McCulloch
// Vehicle Routing

// Instructions to run
//  1) Build the program
//  2) Run the executable generated with the filenames you 
//     would like to generate outputs for.  For example, 
//     "./VehicleRouting vrp_5_4_1 vrp_16_3_1"
//     would generate output files for the the two listed input files.
//  3) The output files will match the names of the input files in the same directory, 
//     but will have "OUTPUT_" before the input file name.  For example, 
//     "./VehicleRouting vrp_5_4_1 vrp_16_3_1"
//     will generate OUTPUT_vrp_5_4_1, OUTPUT_vrp_16_3_1

#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>


#include "iolib.h"
#include "lib.h"
#include "vehicle.h"
#include "customer.h"


// Take arguments from command line
int main(int argc, char* args[]) {

    // Look at each file listed in the command line arguments(did this so I don't have to keep running again and typing different file names)
    // Argument 1 is the "./VehicleRouting" command, so we have to start looking at args[1] to get file names
    for (int fileIndex = 1; fileIndex < argc; fileIndex++) {    

        // Declare a file input stream & file output stream
        std::ifstream fin;
        std::ofstream fout;

        // Assign a name to the output file, it will match the input file  
        // but with "OUTPUT_" at the beginning as to not overwrite it.
        // We need to find the file name WITHOUT the directory
        std::string inputFile, outputFile;

        // Supply with the file name from console, and the inputFile & outputFile to set
        SetIOFiles(args[fileIndex], inputFile, outputFile); 

        // Open the input file
        fin.open(inputFile.data());

        // Assert that the input file successfully opened
        AssertInputFileOpenSuccess(fin.fail(), inputFile);

        // TODO: Map input data to proper data structure
        std::vector<Customer> customers;
        std::vector<Vehicle> vehicles;
        struct Warehouse warehouse;
        InputMapper(fin, customers, vehicles, warehouse);

        // TODO: Solve problem

        // Open the output file
        fout.open(outputFile.data());

        // Output data
        fout << "";

        // Close the input & output files
        fin.close();
        fout.close();
    }

    return 0;
}