//
// Created by Gyebro on 2022-04-19.
//

#ifndef CELL_MAPPING_CPP_LORENZ_H
#define CELL_MAPPING_CPP_LORENZ_H

#include "system.h"
#include "vec3.h"
#include "rk45.h"

class RollingStepan : public cm::DynamicalSystemBase<vec3> {
private:
    double M2, M3, l, R, k, v;
    double dt;
public:
    RollingStepan(double timestep) {
        dt = timestep;
        M2 = 1.5;
        M3 = 3.75;
        l = 0.2;
        R = 0.1;
        k = 75;
        v = 1;
    }
    vec3 f(const vec3 &y0, const double& t) const {
        // Rolling system (Stepan 1991)
        return vec3({
            y0[1],
            -((v*y0[1])/l) - (2.0*k*y0[2])/(l*M2) - 2.0*(1 + pow(l,2)/pow(R,2))*pow(y0[1],2)*y0[0] - (v*y0[1]*pow(y0[0],2))/(2.0*l) + (3.0*k*(1 + (4.0*pow(l,2))/(3.0*pow(R,2)))*y0[2]*pow(y0[0],2))/(l*M2),
            l*y0[1] + v*y0[0] + (l*y0[1]*pow(y0[0],2))/2.0 + (v*pow(y0[0],3))/3.0
        });
    }
    vec3 step(const vec3 &state) const override {
        // Use RK45 to integrate the system scheme
        cm::RK45<vec3,RollingStepan> rk45(this, 1e-7);
        return rk45.step(state, 0, dt, dt/50.0);
    }
};

#endif //CELL_MAPPING_CPP_LORENZ_H
