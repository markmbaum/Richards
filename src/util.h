#ifndef UTIL_H_
#define UTIL_H_

//! \file util.h

#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>

//!swaps two variables
template <class T>
void swap (T *a, T *b) {
    T c;
    c = *a;
    *a = *b;
    *b = c;
}

//!find max of array
double max (double *a, long n);

//!find max magnitude of array
double absmax (double *a, long n);

//!find min of array
double min (double *a, long n);

//!finds the maximum relative difference between two arrays/vectors
template <class T>
double maxreldif (T a, T b, long n) {
    double rd, mrd = 0;
    for (long i=0; i<n; i++) {
        rd = fabs(a[i] - b[i])/fabs(b[i]);
        if ( rd > mrd )
            mrd = rd;
    }
    return(mrd);
}

//!find index of value in array closest to a number
template <class T>
long argclose (T a, double b, long n) {

    double m = INFINITY;
    long i;
    //find min and its index
    long idx = 0;
    for (i=0; i<n; i++) {
        if (fabs(a[i] - b) < m) {
            m = fabs(a[i] - b);
            idx = i;
        }
    }

    return(idx);
}

//!create an evenly spaced vector of values over a range
std::vector<double> linspace (double a, double b, long n);

//!create an logarithmically spaced vector of values over a range
std::vector<double> logspace (double a, double b, long n);

//!subsample a vector, keeping first and last elements
/*!
\param[in] v vector to subsample
\param[in] n approximate length target
*/
template <class T>
std::vector<T> subsample (std::vector<T> v, unsigned long n) {

    //calculate the approximate interval size
    unsigned long size = v.size();
    unsigned long step = size/n;
    //fill a new vector
    if ( (step == 0) || (step == 1) ) {
        //just copy the vector
        std::vector<T> r(v.begin(), v.end());
        //finish
        return(r);
    } else {
        //step is not zero, subsample the vector
        std::vector<T> r;
        r.push_back( v[0] );
        unsigned long i;
        for (i=step; i<size; i+=step) r.push_back(v[i]);
        if (i < size - 1) r.push_back( v.back() );
        //finish
        return(r);
    }
}

#endif
