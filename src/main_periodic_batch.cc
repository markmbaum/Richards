//! \file main_periodic_batch.cc

#include <string>
#include <vector>
#include <iostream>

#include "omp.h"

#include "io.h"
#include "grid.h"
#include "settings.h"
#include "richards.h"

//! driver function compiled into `richards_periodic_batch.exe`
int main (int argc, char **argv) {

    //output csv
    std::string fn;
    FILE *ofile;
    //indices and counters
    long unsigned i, i0, i1, i2, i3, i4, i5, i6, i7, count, nparam;
    //parameter table
    double **param;
    //parameter vectors
    std::vector<double> poro = linspace(0.1, 0.3, 3),
                        perm = logspace(-13, -11, 3),
                        b = linspace(3, 6, 4),
                        wilt = linspace(0.1, 0.5, 4),
                        tauevap = linspace(360, 36000, 12),
                        Levap = linspace(0.1, 0.1, 1),
                        infper = linspace(86400, 8640000, 9),
                        infdur = linspace(1800, 14400, 8);

    if ( argc != 4 )
        print_exit("batch richards must be given two command line arguments\n  1. path to settings file\n  2. domain depth\n  3. path to output directory");

    printf("  starting periodic richards batch integrations\n");

    //output directory
    std::string dirout = argv[3];

    //read settings
    Settings stg = parse_settings(read_values(argv[1]));
    //set the depth
    stg.depth = std::atof(argv[2]);

    //create grid
    Grid grid(stg.depth, stg.delz0, stg.delzfrac, stg.delzmax);
    long n = grid.get_n();
    printf("  %g meter grid created with %li cells\n", grid.get_dep(), grid.get_n());
    printf("    surface cell depth: %g cm\n", grid.get_delze()[n-1]*100);
    printf("    bottom cell depth: %g cm\n", grid.get_delze()[0]*100);
    if ( stg.save_grid )
        grid.save(dirout);

    //create parameter table
    nparam = (poro.size()*perm.size()*b.size()
             *wilt.size()*tauevap.size()*Levap.size()
             *infper.size()*infdur.size());
    param = new double*[nparam];
    for (i=0; i<nparam; i++) param[i] = new double[8];
    fn = dirout + "/trials.csv";
    check_file_write(fn.c_str());
    ofile = fopen(fn.c_str(), "w");
    fprintf(ofile, "trial,poro,perm,b,wilt,tauevap,Levap,infper,infdur\n");
    count = 0;
    for (i0=0; i0<poro.size(); i0++) {
        for (i1=0; i1<perm.size(); i1++) {
            for (i2=0; i2<b.size(); i2++) {
                for (i3=0; i3<wilt.size(); i3++) {
                    for (i4=0; i4<tauevap.size(); i4++) {
                        for (i5=0; i5<Levap.size(); i5++) {
                            for (i6=0; i6<infper.size(); i6++) {
                                for (i7=0; i7<infdur.size(); i7++) {
                                    //fill in a row
                                    param[count][0] = poro[i0];
                                    param[count][1] = perm[i1];
                                    param[count][2] = b[i2];
                                    param[count][3] = wilt[i3];
                                    param[count][4] = tauevap[i4];
                                    param[count][5] = Levap[i5];
                                    param[count][6] = infper[i6];
                                    param[count][7] = infdur[i7];
                                    //write to file
                                    fprintf(ofile, "%lu,%g,%g,%g,%g,%g,%g,%g,%g\n",
                                        count,
                                        param[count][0],
                                        param[count][1],
                                        param[count][2],
                                        param[count][3],
                                        param[count][4],
                                        param[count][5],
                                        param[count][6],
                                        param[count][7]
                                    );
                                    //increment
                                    count++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //close the csv
    fclose(ofile);
    printf("parameter table written to: %s\n", fn.c_str());

    printf("beginning parallel integrations of %lu trials with %d threads\n",
        nparam, omp_get_max_threads());
    printf("     trial    |    nstep\n");
    printf("  ----------- | -----------\n");
    #pragma omp parallel for schedule(dynamic)
    for (long unsigned i=0; i<nparam; i++) {
        //copy settings
        Settings s = copy_settings(stg);
        //edit params
        s.poro = param[i][0];
        s.perm = param[i][1];
        s.b = param[i][2];
        s.wilt = param[i][3];
        s.tauevap = param[i][4];
        s.Levap = param[i][5];
        s.infper = param[i][6];
        s.infdur = param[i][7];
        //create a solver
        Richards rich(grid, s);
        rich.set_quiet(true);
        rich.set_name(int_to_string(i));
        //integrate
        //rich.spinup(1e-6, true);
        //rich.solve_adaptive(2*s.infper, s.infper/1e12, s.nsnap, dirout.c_str());
        printf("  %11lu | %11lu\n", i, rich.get_nstep());
    }

    for (i=0; i<nparam; i++) delete [] param[i];
    delete [] param;

    return(0);
}
