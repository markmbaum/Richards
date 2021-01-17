//! \file richards.cc

#include "richards.h"

Richards::Richards (Grid grid, Settings stgin) :
    OdeSsp3 (grid.get_n()+1),
    stg (copy_settings(stgin)),
    n     (grid.get_n()),
    dep   (grid.get_dep()),
    ze    (grid.get_ze()),
    zc    (grid.get_zc()),
    delz  (grid.get_delz()),
    delze (grid.get_delze()),
    vefac (grid.get_vefac()),
    gefac (grid.get_gefac()) {

    long i;

    //set the name of the object
    set_name("richards");
    //turn on silent snapping
    set_silent_snap(true);

    //------------------
    //physical variables

    //porosity at cell centers
    for (i=0; i<n; i++) poroc.push_back( f_poro(-zc[i], stg.poro) );
    //porosity at cell edges
    for (i=0; i<n+1; i++) poroe.push_back( f_poro(-ze[i], stg.poro) );
    //saturated hydraulic conductivity at cell edges
    for (i=0; i<n+1; i++) Ksat.push_back( f_Ksat(-ze[i], stg.g, stg.mu, stg.rho) );
    //saturated matric head
    for (i=0; i<n+1; i++) psisat.push_back( f_psisat(-ze[i]) );

    //water fractions at cell edges
    we.resize(n+1);
    //derivative of psi w/r/t moisture
    dpsidw.resize(n+1);
    //derivative of moisture w/r/t z
    dwdz.resize(n+1);
    //hydraulic conductivity (unsaturated)
    K.resize(n+1);
    //diffusivity (K*d psi / d t)
    D.resize(n+1, INFINITY);
    //fluxes
    q.resize(n+1);

    //storage vectors, if needed
    if ( stg.qall ) qall.resize(n+1);
    if ( stg.wall ) wall.resize(n);

    //find the approximate middle cell edge
    mididx = argclose(ze, -dep/2.0, n+1);

    //-------------------------------------
    //discretization constants for stable dt

    for (i=0; i<n+1; i++) dtcons.push_back( delze[i]*delze[i]/2.0 );

    //------------------
    //initial condition

    //water fractions
    for (i=0; i<n; i++) set_sol(i, 0.999*poroc[i]);
    //time
    set_sol(n, 0.0);

}

//------------------------------------------------------------------------------
//physical functions

double Richards::f_poro (double depth, double poro) {
    (void)depth;
    return(
        poro
    );
}

double Richards::f_ksat (double depth, double perm) {
    (void)depth;
    return(
        perm
    );
}

double Richards::f_Ksat (double depth, double g, double mu, double rho) {
    return ( rho*g*f_ksat(depth, stg.perm)/mu );
}

double Richards::f_K (double w, double Ksat, double wsat, double b) {
    return(
        Ksat*pow(w/wsat, 2.0*b + 3.0)
    );
}

double Richards::f_psisat (double depth) {
    (void)depth;
    return(
        -0.2
    );
}

double Richards::f_dpsidw (double w, double psisat, double wsat, double b) {
    return(
        -(b/wsat)*psisat*pow(w/wsat, -(b + 1))
    );
}

bool Richards::f_infil (double t) {

    //bounds of next/current infiltration event
    double ta, tb;
    infil_times(t, &ta, &tb);
    //check if current time is in the event window
    if ( (t >= ta) && (t <= tb) )
        //make it rain
        return(true);
    //let it dry
    return(false);
}

double Richards::f_w_bot (double poro) {
    return(
        poro
    );
}

//------------------------------------------------------------------------------
//ODE solver functions

void Richards::infil_times (double t, double *ta, double *tb) {
    //end of next infiltration event
    (*tb) = t - fmod(t, stg.infper) + stg.infper;
    //beginning of next infiltration event
    (*ta) = (*tb) - stg.infdur;
}

double Richards::f_q (double K, double dpsidw, double dwdz, double satl, double satr) {

    double q = -K*dpsidw*dwdz - K;

    if ( q < 0 ) {
        if ( (satl > 1) || (satr < stg.wilt) )
            q = 0.0;
    } else if ( q > 0 ) {
        if ( (satr > 1) || (satl < stg.wilt) )
            q = 0.0;
    }

    return(q);
}

double Richards::f_dwdt (double qt, double qb, double delz) {
    return(
        (qt - qb)/delz
    );
}

void Richards::update_q (double *w, double t) {

    //index
    long i;
    //infiltration flag
    bool infil = f_infil(t);

    //----------------------------------------------
    //cell edge saturations and saturation gradients

    //bottom
    we[0] = f_w_bot(poroe[0]);
    dwdz[0] = (w[0] - we[0])/(delz[0]/2);
    //interior
    for (i=1; i<n; i++) {
        we[i] = vefac[i]*w[i] + (1.0 - vefac[i])*w[i-1];
        dwdz[i] = gefac[i]*(w[i] - w[i-1]);
    }
    //top value depends on infiltration flag
    infil ? we[n] = poroe[n] : NAN;
    dwdz[n] = (we[n] - w[n-1])/(delz[n-1]/2);

    //------------------------------
    //cell edge hydraulic properties

    for (i=0; i<n+1; i++) {
        K[i] = f_K(we[i], Ksat[i], poroe[i], stg.b);
        dpsidw[i] = f_dpsidw(we[i], psisat[i], poroe[i], stg.b);
        D[i] = K[i]*dpsidw[i];
    }

    //------
    //fluxes

    //water table
    q[0] = f_q(K[0], dpsidw[0], dwdz[0], stg.wilt, w[0]/poroc[0]);
    //cell-to-cell exchange
    for (i=1; i<n; i++)
        q[i] = f_q(K[i], dpsidw[i], dwdz[i], w[i-1]/poroc[i-1], w[i]/poroc[i-1]);
    //surface infiltration/evaporation
    if ( infil ) {
        q[n] = f_q(K[n], dpsidw[n], dwdz[n], w[n-1]/poroc[n-1], stg.wilt);
    } else {
        q[n] = stg.Levap*(w[n-1] - stg.wilt*poroe[n])/stg.tauevap;
    }
}

void Richards::ode_fun (double *solin, double *fout) {

    //autonomous form for time
    fout[n] = 1.0;
    //compute fluxes
    update_q(solin, solin[n]);
    //time derivatives of water fractions
    for (long i=0; i<n; i++)
        fout[i] = f_dwdt(q[i], q[i+1], delz[i]);
}

double Richards::dt_adapt () {

    //compute fraction of maximum stable time step
    double dt = INFINITY;
    double dtmax;
    for (long i=0; i<n+1; i++) {
        dtmax = dtcons[i]/D[i];
        if ( (dtmax < dt) && !std::isnan(dtmax) )
            dt = dtmax;
    }
    dt *= stg.dtfac;
    //infiltration management
    double t, ta, tb;
    t = get_sol(n);
    infil_times(get_sol(n), &ta, &tb);
    if ( (t >= ta) && (t <= tb) ) {
        if ( dt > stg.infdur/1000 )
            dt = stg.infdur/1000;
        if ( t + dt > tb )
            dt = tb - t;
    } else {
        if ( t + dt > ta )
            dt = ta - t;
    }
    //evaporation management
    if ( dt > stg.tauevap*delz[0]/stg.Levap )
        dt = 0.99*stg.tauevap*delz[0]/stg.Levap;

    return( dt );
}

void Richards::steady (double atol, unsigned long ntol) {

    //index
    long i;
    //step counter
    unsigned long count = 0;
    //euler time step
    double dt = INFINITY;
    //alias the solution array
    double *w = get_sol();
    //storage for time derivatives, initially at inf
    double *dwdt = new double[n+1];
    for (i=0; i<n; i++) dwdt[i] = INFINITY;

    //integrate until dwdt is small enough or too many steps are taken
    while ( (absmax(dwdt, n)*dt > atol) && (count < ntol) ) {
        ode_fun(w, dwdt);
        dt = 0.75*dt_adapt();
        for (i=0; i<n; i++)
            w[i] += dt*dwdt[i];
        count++;
    }

    delete [] dwdt;
}

void Richards::spinup (double rtol, bool quiet) {

    long i;
    if ( !quiet ) {
        printf("  max rel dif requirement is %g\n", rtol);
        printf("    PERIODS |  MAX REL DIF \n");
        printf("    ------- | -------------\n");
    }
    //integrate over a single infiltration period to get started
    solve_adaptive(stg.infper, stg.infper/1e12, false);
    //store the bottom boundary flux
    std::vector<double> q_a(n, INFINITY);
    std::vector<double> q_b;
    for (i=0; i<n; i++) q_b.push_back(q[i]);
    //maximum relative difference
    double mrd = maxreldif(q_a, q_b, n);
    //continue integrating over infiltration periods until the qb is stable
    long count = 0;
    long ord = floor(log10(mrd));
    while ( (mrd > rtol) || (count <= 5) ) {
        solve_adaptive(stg.infper, stg.infper/1e12, false);
        count++;
        for (i=0; i<n; i++) {
            q_a[i] = q_b[i];
            q_b[i] = q[i];
        }
        mrd = maxreldif(q_a, q_b, n);
        if ( (!quiet) && (floor(log10(mrd)) != ord ) )  {
            printf("    %-7li | %-11g\n", count, mrd);
            ord = floor(log10(mrd));
        }
    }
}

//------------------------------------------------------------------------------
//extras

void Richards::before_solve () {
    std::string name = get_name();
    std::string dirout = get_dirout();
    if ( stg.poroc )
        write_array(dirout + "/" + name + "_poroc", poroc);
    if ( stg.poroe )
        write_array(dirout + "/" + name + "_poroe", poroe);
    if ( stg.Ksat )
        write_array(dirout + "/" + name + "_Ksat", Ksat);
    if ( stg.psisat )
        write_array(dirout + "/" + name + "_psisat", psisat);
}

void Richards::after_snap (std::string dirout, long isnap, double tin) {
    std::string name = get_name();
    std::string i = int_to_string(isnap);
    if ( stg.we )
        write_array(dirout + "/" + name + "_we_" + i, we);
    if ( stg.dpsidw )
        write_array(dirout + "/" + name + "_dpsidw_" + i, dpsidw);
    if ( stg.dwdz )
        write_array(dirout + "/" + name + "_dwdz_" + i, dwdz);
    if ( stg.K )
        write_array(dirout + "/" + name + "_K_" + i, K);
    if ( stg.D )
        write_array(dirout + "/" + name + "_D_" + i, D);
    if ( stg.w )
        write_array(dirout + "/" + name + "_w_" + i, get_sol(), n);
    if ( stg.q )
        write_array(dirout + "/" + name + "_q_" + i, q);
    if ( stg.tsnap )
        tsnap.push_back( tin );
}

void Richards::q_updated (double t, bool *qup) {

    if ( !(*qup) ) {
        update_q(get_sol(), t);
        *qup = true;
    }
}

void Richards::after_step (double tin) {

    //q updated flag
    bool qup = false;

    if ( stg.t )
        t.push_back( tin );
    if ( stg.qtop ) {
        q_updated(tin, &qup);
        qtop.push_back( q.back() );
    }
    if ( stg.qmid ) {
        q_updated(tin, &qup);
        qmid.push_back( q[mididx] );
    }
    if ( stg.qbot ) {
        q_updated(tin, &qup);
        qbot.push_back( q[0] );
    }
    if ( stg.qall ) {
        q_updated(tin, &qup);
        for (long i=0; i<n+1; i++) qall[i].push_back( q[i] );
    }
    if ( stg.wall )
        for (long i=0; i<n; i++) wall[i].push_back( get_sol(i) );
    if ( stg.infil )
        infil.push_back( f_infil(tin) );
}

void Richards::after_solve () {

    std::string name = get_name();
    std::string dirout = get_dirout();
    std::vector<float> all, temp;
    if ( stg.tsnap )
        write_array(dirout + "/" + name + "_tsnap", tsnap);
    if ( stg.t )
        write_array(dirout + "/" + name + "_t", subsample(t, stg.nmaxout));
    if ( stg.qtop )
        write_array(dirout + "/" + name + "_qtop", subsample(qtop, stg.nmaxout));
    if ( stg.qmid )
        write_array(dirout + "/" + name + "_qmid", subsample(qmid, stg.nmaxout));
    if ( stg.qbot )
        write_array(dirout + "/" + name + "_qbot", subsample(qbot, stg.nmaxout));
    if ( stg.qall ) {
        all.clear();
        for (long i=0; i<n+1; i++) {
            temp = subsample(qall[i], stg.nmaxout);
            for (long unsigned j=0; j<temp.size(); j++)
                all.push_back(temp[j]);
        }
        write_array(dirout + "/" + name + "_qall", all);
    }
    if ( stg.wall ) {
        all.clear();
        for (long i=0; i<n; i++) {
            temp = subsample(wall[i], stg.nmaxout);
            for (long unsigned j=0; j<temp.size(); j++)
                all.push_back(temp[j]);
        }
        write_array(dirout + "/" + name + "_wall", all);
    }
    if ( stg.infil )
        write_array(dirout + "/" + name + "_infil", subsample(infil, stg.nmaxout));
}
