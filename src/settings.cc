//! \file settings.cc

#include "settings.h"

bool eval_txt_bool (const char *s) {
    if ( (strcmp(s, "true") == 0) || (strcmp(s, "True") == 0) ) return(true);
    return(false);
}

bool cmp (const char *s1, const char *s2) {
    if ( strcmp(s1, s2) == 0 ) return(true);
    return(false);
}

long to_long(const char *val) {
    //convert a string to a long int, handling scientific notation
    return( long(std::atof(val)) );
}

Settings parse_settings ( std::vector< std::vector< std::string > > sv ) {

    Settings s;
    const char *set, *val;

    for (int i=0; i < int(sv.size()); i++) {

        //get the setting and value pair
        set = sv[i][0].c_str();
        val = sv[i][1].c_str();

        //get the setting

        if      ( cmp(set, "depth") ) s.depth = std::atof(val);
        else if ( cmp(set, "delz0") ) s.delz0 = std::atof(val);
        else if ( cmp(set, "delzfrac") ) s.delzfrac = std::atof(val);
        else if ( cmp(set, "delzmax") ) s.delzmax = std::atof(val);
        else if ( cmp(set, "save_grid") ) s.save_grid = eval_txt_bool(val);

        else if ( cmp(set, "tint") ) s.tint = std::atof(val);
        else if ( cmp(set, "tunit") ) s.tunit = std::atof(val);
        else if ( cmp(set, "nsnap") ) s.nsnap = to_long(val);
        else if ( cmp(set, "nmaxout") ) s.nmaxout = to_long(val);
        else if ( cmp(set, "dtfac") ) s.dtfac = std::atof(val);

        else if ( cmp(set, "poro") ) s.poro = std::atof(val);
        else if ( cmp(set, "perm") ) s.perm = std::atof(val);
        else if ( cmp(set, "g") ) s.g = std::atof(val);
        else if ( cmp(set, "mu") ) s.mu = std::atof(val);
        else if ( cmp(set, "rho") ) s.rho = std::atof(val);
        else if ( cmp(set, "b") ) s.b = std::atof(val);
        else if ( cmp(set, "wilt") ) s.wilt = std::atof(val);

        else if ( cmp(set, "tauevap") ) s.tauevap = std::atof(val);
        else if ( cmp(set, "Levap") ) s.Levap = std::atof(val);
        else if ( cmp(set, "infper") ) s.infper = std::atof(val);
        else if ( cmp(set, "infdur") ) s.infdur = std::atof(val);

        else if ( cmp(set, "poroc") ) s.poroc = eval_txt_bool(val);
        else if ( cmp(set, "poroe") ) s.poroe = eval_txt_bool(val);
        else if ( cmp(set, "Ksat") ) s.Ksat = eval_txt_bool(val);
        else if ( cmp(set, "psisat") ) s.psisat = eval_txt_bool(val);
        else if ( cmp(set, "dpsidw") ) s.dpsidw = eval_txt_bool(val);
        else if ( cmp(set, "dwdz") ) s.dwdz = eval_txt_bool(val);
        else if ( cmp(set, "K") ) s.K = eval_txt_bool(val);
        else if ( cmp(set, "D") ) s.D = eval_txt_bool(val);
        else if ( cmp(set, "w") ) s.w = eval_txt_bool(val);
        else if ( cmp(set, "we") ) s.we = eval_txt_bool(val);
        else if ( cmp(set, "wall") ) s.wall = eval_txt_bool(val);
        else if ( cmp(set, "q") ) s.q = eval_txt_bool(val);
        else if ( cmp(set, "qtop") ) s.qtop = eval_txt_bool(val);
        else if ( cmp(set, "qmid") ) s.qmid = eval_txt_bool(val);
        else if ( cmp(set, "qbot") ) s.qbot = eval_txt_bool(val);
        else if ( cmp(set, "qall") ) s.qall = eval_txt_bool(val);
        else if ( cmp(set, "infil") ) s.infil = eval_txt_bool(val);
        else if ( cmp(set, "t") ) s.t = eval_txt_bool(val);
        else if ( cmp(set, "tsnap") ) s.tsnap = eval_txt_bool(val);

        else {
            std::cout << "FAILURE: unknown setting in settings file: " << set << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    return(s);
}

Settings copy_settings (Settings &b) {
    //blank struct
    Settings a;
    //grid
    a.depth = b.depth;
    a.delz0 = b.delz0;
    a.delzfrac = b.delzfrac;
    a.delzmax = b.delzmax;
    a.save_grid = b.save_grid;
    //model
    a.tint = b.tint;
    a.tunit = b.tunit;
    a.nsnap = b.nsnap;
    a.nmaxout = b.nmaxout;
    a.dtfac = b.dtfac;
    //physical
    a.poro = b.poro;
    a.perm = b.perm;
    a.g = b.g;
    a.mu = b.mu;
    a.rho = b.rho;
    a.b = b.b;
    a.wilt = b.wilt;
    //forcing
    a.tauevap = b.tauevap;
    a.Levap = b.Levap;
    a.infper = b.infper;
    a.infdur = b.infdur;
    //trackers and output
    a.poroc = b.poroc;
    a.poroe = b.poroe;
    a.Ksat = b.Ksat;
    a.psisat = b.psisat;
    a.dpsidw = b.dpsidw;
    a.dwdz = b.dwdz;
    a.K = b.K;
    a.D = b.D;
    a.w = b.w;
    a.we = b.we;
    a.wall = b.wall;
    a.q = b.q;
    a.qtop = b.qtop;
    a.qmid = b.qmid;
    a.qbot = b.qbot;
    a.qall = b.qall;
    a.infil = b.infil;
    a.t = b.t;
    a.tsnap = b.tsnap;

    return(a);
}
