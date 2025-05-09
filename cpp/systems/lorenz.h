//
// Created by Gyebro on 2022-04-19.
//

#ifndef CELL_MAPPING_CPP_LORENZ_H
#define CELL_MAPPING_CPP_LORENZ_H

#include "system.h"
#include "vec3.h"
#include "rk45.h"

class Lorenz : public cm::DynamicalSystemBase<vec3> {
private:
    double rho;
    double sigma;
    double beta;
    double dt;
public:
    Lorenz(double Rho, double Sigma, double Beta, double timestep) : rho(Rho), sigma(Sigma), beta(Beta) {
        dt = timestep;
    }
    vec3 f(const vec3 &y0, const double& t) const {
        // Classic Lorenz attractor
        return vec3({
            sigma*(y0[1]-y0[0]),
            y0[0]*(rho-y0[2])-y0[1],
            y0[0]*y0[1]-beta*y0[2]
        });
    }
    vec3 step(const vec3 &state) const override {
        // Use RK45 to integrate the system scheme
        cm::RK45<vec3,Lorenz> rk45(this, 1e-8);
        return rk45.step(state, 0, dt, dt/50.0);
    }
};

class LimitCycle : public cm::DynamicalSystemBase<vec3> {
private:
    double dt;
public:
    LimitCycle(double timestep) {
        dt = timestep;
    }
    vec3 f(const vec3 &y0, const double& t) const {
        // Modified double-tubular limit cycle
        return vec3({
            2000.0-3.0*y0[0]-300.0*(y0[1]*y0[1])-10000.0*(y0[2]*y0[2]),
            y0[1]+2.0*y0[2]+y0[0]*(y0[1]+(4.0/3.0)*y0[2]),
            -2.0*y0[1]+y0[2]+y0[0]*(-(4.0/3.0)*y0[1]+y0[2])
        });
    }
    vec3 step(const vec3 &state) const override {
        // Use RK45 to integrate the system scheme
        cm::RK45<vec3,LimitCycle> rk45(this, 1e-8);
        return rk45.step(state, 0, dt, dt/50.0);
    }
};


#endif //CELL_MAPPING_CPP_LORENZ_H
