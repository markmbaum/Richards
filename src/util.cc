//! \file util.cc

#include "util.h"

//!number of seconds in an Earth year
#define YRSEC 31557600.0

double max (double *a, long n) {
    double r = 0.0;
    long i;
    //first find a minimum or let m stay at zero
    for (i=0; i<n; i++) if (a[i] < r) r = a[i];
    //now that m is less than or equal to the lowest element of a, find the max
    for (i=0; i<n; i++) if (a[i] > r) r = a[i];
    return(r);
}

double absmax (double *a, long n) {
    double r = 0.0;
    long i;
    //now that m is less than or equal to the lowest element of a, find the max
    for (i=0; i<n; i++) if (fabs(a[i]) > r) r = fabs(a[i]);
    return(r);
}

double min (double *a, long n) {
    double r = 0.0;
    long i;
    //first find a maximum or let m stay at zero
    for (i=0; i<n; i++) if (a[i] > r) r = a[i];
    //now that m is greater than or equal to the largest element of a, find the min
    for (i=0; i<n; i++) if (a[i] < r) r = a[i];
    return(r);
}

std::vector<double> linspace (double a, double b, long n) {

    //avoid division by zero
    if ( n == 1 ) {
        if ( a != b ) {
            printf("FAILURE: cannot linspace with a single value if the range limits are not identical");
            exit(EXIT_FAILURE);
        }
        std::vector<double> v(1, a);
        return(v);
    }
    //spacing
    double h = (b - a)/(n - 1);
    //values
    std::vector<double> v;
    for (long i=0; i<n; i++)
        v.push_back( a + i*h );

    return(v);
}

std::vector<double> logspace (double a, double b, long n) {

    //avoid division by zero
    if ( n == 1 ) {
        if ( a != b ) {
            printf("FAILURE: cannot logspace with a single value if the range limits are not identical");
            exit(EXIT_FAILURE);
        }
        std::vector<double> v(1, pow(10, a));
        return(v);
    }
    //spacing
    double h = (b - a)/(n - 1);
    //values
    std::vector<double> v;
    for (long i=0; i<n; i++)
        v.push_back( pow(10, a + i*h) );

    return(v);
}
