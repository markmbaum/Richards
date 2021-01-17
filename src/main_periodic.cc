//! \file main_periodic.cc

#include <string>
#include <vector>
#include <iostream>

#include "io.h"
#include "grid.h"
#include "settings.h"
#include "richards.h"

//! driver function compiled into `richards_periodic.exe`
int main (int argc, char **argv) {

    if ( argc != 3 )
        print_exit("richards must be given two command line arguments\n  1. path to settings file\n  2. path to output directory");

    printf("  starting periodic richards integration\n");

    //output directory
    std::string dirout = argv[2];

    //read settings
    Settings stg = parse_settings(read_values(argv[1]));

    //create grid
    Grid grid(stg.depth, stg.delz0, stg.delzfrac, stg.delzmax);
    long n = grid.get_n();
    printf("  %g meter grid created with %li cells\n", grid.get_dep(), grid.get_n());
    printf("    surface cell depth: %g cm\n", grid.get_delze()[n-1]*100);
    printf("    bottom cell depth: %g cm\n", grid.get_delze()[0]*100);
    if ( stg.save_grid )
        grid.save(dirout);

    //create system
    Richards rich(grid, stg);

    //integrate
    printf("  spinning up...\n");
    rich.spinup(1e-6, false);
    printf("  spinup finished @ t = %g, doing short integration\n", rich.get_t());
    long unsigned nstep = rich.get_nstep();
    rich.solve_adaptive(2*stg.infper, stg.infper/1e12, stg.nsnap, dirout.c_str());
    printf("  %lu short integration steps\n", rich.get_nstep() - nstep);
    printf("  %lu total steps\n", rich.get_nstep());
    printf("  done\n");

    return(0);
}
