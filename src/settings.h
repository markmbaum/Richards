#ifndef SETTINGS_H_
#define SETTINGS_H_

//! \file settings.h

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <cstdlib>

//!container struct for all the settings variables needed for a BousThermModel run
struct Settings {

    //-------------------------------------
    //grid settings

    //!domain depth (m)
    double depth;
    //!surface cell width (m)
    double delz0;
    //!fraction increase for deeper neighbor cell
    double delzfrac;
    //!maximum cell width (m)
    double delzmax;
    //!whether to write grid files
    bool save_grid;

    //-------------------------------------
    //model set up and integration settings

    //!duration of solve
    double tint;
    //!time unit to use (number of seconds)
    double tunit;
    //!number of snaps to take
    long nsnap;
    //!maximum length of output vectors (subsampled to accomodate)
    long nmaxout;
    //!safety factor for stable time step
    double dtfac;

    //-------------------------------------
    //physical parameters

    //!porosity
    double poro;
    //!permeability (m^2)
    double perm;
    //!gravity (m/s^2)
    double g;
    //!water viscosity (Pa*s)
    double mu;
    //!water density (kg/m^3)
    double rho;
    //!Brooks-Corey parameter
    double b;
    //!wilting saturation as fraction of porosity
    double wilt;

    //-------------------------------------
    //periodic forcing

    //!evaporation timescale
    double tauevap;
    //!evaporation depth
    double Levap;
    //!infiltration period (s)
    double infper;
    //!infiltration duration (s)
    double infdur;

    //-------------------------------------
    //trackers and output

    //!whether to write porosity at cell centers
    bool poroc;
    //!whether to write porosity at cell edges
    bool poroe;
    //!whether to write saturated hydraulic conductivity
    bool Ksat;
    //!whether to write saturated matric head
    bool psisat;
    //!whether to write dpsidw when snapping
    bool dpsidw;
    //!whether to write moisture fraction derivative when snapping
    bool dwdz;
    //!whether to write hydraulic conductivity when snapping
    bool K;
    //!whether to write diffusivity when snapping
    bool D;
    //!whether to water fraction profile when snapping
    bool w;
    //!whether to write water fraction at cell edges when snapping
    bool we;
    //!whether to track all water fractions
    bool wall;
    //!whether to write water flux when snapping
    bool q;
    //!whether to track top flux
    bool qtop;
    //!whether to track middle flux
    bool qmid;
    //!whether to track bottom flux
    bool qbot;
    //!whether to track all fluxes
    bool qall;
    //!whether to track infiltration flag
    bool infil;
    //!whether to track step times
    bool t;
    //!whether to track snap times
    bool tsnap;

};

//!converts a string into a bool if it's supposed to be "true" or "false"
bool eval_txt_bool (const char *s);

//!abbreviates strcmp
bool cmp (const char *s1, const char *s2);

//!converts a character to an integet
long to_long(const char *val);

//!parses a settings file and returns it in a Settings structure
/*!
\param[in] sv vector of vectors of strings from read_values_file()
*/
Settings parse_settings ( std::vector< std::vector< std::string > > sv );

//!constructs a copy of another Settings object
/*!
\param[in] b the Settings object to copy
*/
Settings copy_settings (Settings &b);

#endif
