//
//  problem.c
//  
//
//  Created by Noah Ferich on 2/13/21.
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "rebound.h"
#include "reboundx.h"

int main(int argc, char* argv[]) {

struct reb_simulation* sim = reb_create_simulation(); //creates simulation

sim->G = 4*M_PI*M_PI;  // use units of AU, yr and solar masses
sim->dt = .05;         //timestep for simulation in yrs
sim->integrator = REB_INTEGRATOR_WHFAST; //integrator for sim

//following adds star with mass of Sun to sim
struct reb_particle star = {0};
star.m = 1.;
reb_add(sim, star);

//following variables are the orbital elements of only asteroid in sim
double m = 0;
double a = .5;
double e = 0;
double inc = 0;
double Omega = 0;
double omega = 0;
double f = 0;

//adds asteroid to the sim
struct reb_particle asteroid_1 = reb_tools_orbit_to_particle(sim->G, star, m, a, e, inc, Omega, omega, f);

reb_add(sim,asteroid_1);
    
struct reb_particle* const particles = sim->particles; //pointer for the particles in the sim

struct rebx_extras* rebx = rebx_attach(sim);
struct rebx_force* yark = rebx_load_force(rebx, "yarkovsky_effect");
    
double au_conv = 1.495978707e11;
double msun_conv = 1.9885e30;
double yr_conv = 31557600.0;

double density = (3000.0*au_conv*au_conv*au_conv)/msun_conv;
double c = (2.998e8*yr_conv)/au_conv;
double lstar = (3.828e26*yr_conv*yr_conv*yr_conv)/(msun_conv*au_conv*au_conv);
double albedo = .017;
double stef_boltz = ((5.670e-8)*yr_conv*yr_conv*yr_conv)/(msun_conv);
double emissivity = .9;
double k = 1.0;
double C = (680*yr_conv*yr_conv)/(au_conv*au_conv);
double K = (.04*yr_conv*yr_conv*yr_conv)/(au_conv*msun_conv);
double rotation_period = 15470.9/yr_conv;
    
rebx_set_param_double(rebx, &sim->particles[1].ap, "body_density", density);
rebx_set_param_int(rebx, &sim->particles[1].ap, "yark_flag", 0);
rebx_set_param_double(rebx, &yark->ap, "lstar", lstar);
rebx_set_param_double(rebx, &yark->ap, "yark_c", c);
particles[1].r = 1000/au_conv;

rebx_set_param_double(rebx, &sim->particles[1].ap, "albedo", albedo);
rebx_set_param_double(rebx, &yark->ap, "stef_boltz", stef_boltz);
rebx_set_param_double(rebx, &sim->particles[1].ap, "emissivity", emissivity);
rebx_set_param_double(rebx, &sim->particles[1].ap, "k", k);
rebx_set_param_double(rebx, &sim->particles[1].ap, "specific_heat_capacity", C);
rebx_set_param_double(rebx, &sim->particles[1].ap, "thermal_conductivity", K);
rebx_set_param_double(rebx, &sim->particles[1].ap, "rotation_period", rotation_period);

rebx_add_force(rebx, yark);

double tmax = 10;
    
reb_integrate(sim, tmax); //integrates system for tmax years
    
struct reb_orbit o= reb_tools_particle_to_orbit(sim->G, sim->particles[1], sim->particles[0]); //o gives orbital parameters for asteroid after sim
    
double final_a = o.a; //final semi-major axis of asteroid after sim
    
printf("CHANGE IN SEMI-MAJOR AXIS: %1.30f\n", (final_a-a)); //prints difference between the intitial and final semi-major axes of asteroid
    
}