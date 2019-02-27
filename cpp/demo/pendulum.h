//
// Created by Gyebro on 2019-02-27.
//

#ifndef CELL_MAPPING_CPP_PENDULUM_H
#define CELL_MAPPING_CPP_PENDULUM_H

#include "system.h"
#include "vec2.h"
#include "rk45.h"

/**
 * \brief Single degree-of-freedom pendulum
 * q'' + beta q' = - alpha sin(q)
 * Using y0 = q, y1 = q', y = [y0 y1]
 * y'' = -beta y' - alpha sin(y)
 * y'  = y'
 */
class Pendulum : public cm::DynamicalSystemBase<vec2>  {
private:
    double alpha;
    double delta;
    double dt;
public:
    Pendulum(double alpha, double delta, double timestep) : alpha(alpha), delta(delta) {
        dt = timestep;
    }
    /**
    * \brief Yields the derivative of the state vector (used by RK45)
    */
    vec2 f(const vec2 &y0, const double& t) const {
        return vec2({
                     y0[1],
                     -delta*y0[1] -alpha*sin(y0[0])
                    });
    }
    vec2 step(const vec2 &state) const override {
        // Use RK45 to integrate the system scheme
        cm::RK45<vec2,Pendulum> rk45(this, 1e-8);
        return rk45.step(state, 0, dt, dt/50.0);
    }

};


#endif //CELL_MAPPING_CPP_PENDULUM_H
