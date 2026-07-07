#include <iostream>
#include <string>
#include <fstream>
#include "system.h"

using namespace std;

int main (int argc, char *argv[]){

    if(argc < 2){
        cerr << "Usage: ./simulator.exe RUN_NAME" << endl;
        cerr << "Example: ./simulator.exe liquid_MC_equilibration" << endl;
        return 1;
    }

    string run_name = argv[1];

    System SYS(run_name);

    SYS.initialize();

    // EQUILIBRATION
    int neq = 0;
    for(int i=0; i<neq; i++){
    SYS.step();
    }

    SYS.initialize_properties();

    for(int i=0; i<SYS.get_nbl(); i++){
    SYS.block_reset(i+1);

    for(int j=0; j<SYS.get_nsteps(); j++){
        SYS.step();
        SYS.measure();
    }

    SYS.averages(i+1);
    }

    SYS.finalize();

    return 0;
}