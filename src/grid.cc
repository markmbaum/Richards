//! \file grid.cc

#include "grid.h"

Grid::Grid (double depth, double delz0, double delzfrac, double delzmax) {

    long i;

    //compute grid edges
    grid_edges(depth, delz0, delzfrac, delzmax, ze);

    //number of cells
    n = ze.size() - 1;

    //assign the domain length/depth
    dep = -ze[0];

    //compute cell center coordinates
    for (i=0; i<n; i++) zc.push_back( ze[i+1]/2.0 + ze[i]/2.0 );

    //compute cell widths
    for (i=0; i<n; i++) delz.push_back( ze[i+1] - ze[i] );

    //cell widths used for stability calculations
    delze.push_back( delz[0] ); //bottom edge looks at bottom cell
    for (i=1; i<n; i++) delze.push_back( delz[i] );
    delze.push_back( delz[n-1] );

    //factors for cell edge interpolation
    vefac.push_back( NAN );
    for (i=1; i<n; i++) vefac.push_back( (ze[i] - zc[i-1])/(zc[i] - zc[i-1]) );
    vefac.push_back( NAN );

    //factors for cell edge gradients
    gefac.push_back( NAN );
    for (i=1; i<n; i++) gefac.push_back( 1.0/(zc[i] - zc[i-1]) );
    gefac.push_back( NAN );

}

void Grid::save (std::string dirout) {
    write_array(dirout + "/zc", zc);
    write_array(dirout + "/ze", ze);
    write_array(dirout + "/delz", delz);
    write_array(dirout + "/delze", delze);
    write_array(dirout + "/vefac", vefac);
    write_array(dirout + "/gefac", gefac);
}

void Grid::grid_edges(double depth, double delz0, double delzfrac,
                      double delzmax, std::vector<double> &ze) {

    double delz, f, tem;
    long i,n;

    //start with the top cell edge
    ze.clear();
    ze.push_back( 0.0 );
    ze.push_back( delz0 );
    //fill the vector with increasingly large cells
    while ( ze.back() < depth ) {
        delz = (ze.back() - ze.end()[-2])*delzfrac;
        if ( delz < delzmax ) {
            ze.push_back( ze.back() + delz );
        } else {
            ze.push_back( ze.back() + delzmax );
        }
    }
    n = long(ze.size());
    //squeeze cells down to get the depth correct
    f = depth/ze.back();
    for (i=0; i<n; i++)
        ze[i] *= -f;
    //swap the order
    for (i=0; i<n/2; i++) {
        tem = ze[i];
        ze[i] = ze[n-i-1];
        ze[n-i-1] = tem;
    }
}
