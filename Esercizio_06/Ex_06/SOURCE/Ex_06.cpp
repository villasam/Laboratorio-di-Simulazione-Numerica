#include <iostream>
#include <string>
#include <fstream>
#include "system.h"

using namespace std;

int main (int argc, char *argv[]){

    if(argc < 2){
        cerr << "Usage: ./simulator.exe RUN_NAME" << endl;
        cerr << "Example: ./simulator.exe metro_h0_T1.00" << endl;
        return 1;
    }

    string run_name = argv[1];

    System SYS(run_name);
    SYS.initialize();
    SYS.initialize_properties();
    SYS.block_reset(0);

    for(int i=0; i < SYS.get_nbl(); i++){
        for(int j=0; j < SYS.get_nsteps(); j++){
        SYS.step();
        SYS.measure();
        }
        SYS.averages(i+1);
        SYS.block_reset(i+1);
    }

    SYS.finalize();

    return 0;
}