//! \file main.cc

#include <string>
#include <vector>
#include <iostream>

#include "io.h"
#include "grid.h"
#include "settings.h"
#include "richards.h"

//! driver function compiled into `richards.exe`
int main (int argc, char **argv) {

    if ( argc != 3 )
        print_exit("richards must be given two command line arguments\n  1. path to settings file\n  2. path to output directory");

    printf("  starting richards integration from initial condition\n");

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
    double tint = stg.tint*stg.tunit;
    rich.solve_adaptive(tint, tint/1e9, stg.nsnap, dirout.c_str());
    printf("  done\n");

    return(0);
}
