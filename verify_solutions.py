import pathlib
import os
import platform
import subprocess
from os import listdir
from os.path import isfile, join

if __name__ == '__main__':

    # Get list of input files and their outputs
    project_directory = os.path.realpath(os.path.dirname(__file__))
    mypath = pathlib.Path(__file__).parent.resolve() / 'data'
    input_files = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    inputs = input_files[0:int(len(input_files) / 2)]
    outputs = input_files[int(len(input_files) / 2):int(len(input_files))]

    # determine mac or windows, set as var for debug
    os_name = platform.system()
    if (os_name == "Windows"):
        command = "verifier.exe "
    else:
        command = "./verifier "

    # add files to commands
    project_path = os.path.realpath(os.path.dirname(__file__))
    # Compile source
    if (os_name == "Windows"):
        subprocess.run("g++ -g src/verifier.cpp -o verifier.exe")
        subprocess.run(command)
    else:
        os.system("g++ -g src/verifier.cpp -o verifier")
        os.system(command)

    for i in range(0, len(outputs)):
        input_file = inputs[i]
        output_file = outputs[i]
        subprocess.run(command + "data/" + outputs[i] + " " + "data/" + inputs[i] + " ")
    #subprocess.run("verifier.exe data/vrp_51_5_1 data/OUTPUT_vrp_51_5_1")