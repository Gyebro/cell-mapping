#ifndef CELL_MAPPING_CPP_DUFFING_H
#define CELL_MAPPING_CPP_DUFFING_H

#include "system.h"
#include "vec2.h"
#include "rk45.h"

/**
 * \brief Modified Duffing Oscillator
 * q'' + delta q' + alpha q + beta q^3 = gamma cos(omega t)
 * Using y0 = q, y1 = q', y = [y0 y1]
 * y'' = -delta y' - alpha y - beta y^3 + gamma cos(omega t)
 * y'  = y'
 */
class DuffingOscillator : public cm::DynamicalSystemBase<vec2>  {
private:
    double alpha;
    double beta;
    double gamma;
    double delta;
    double omega;
    double dt;
public:
    DuffingOscillator(double alpha, double beta, double gamma, double delta, double omega, double timestep) : alpha(
            alpha), beta(beta), gamma(gamma), delta(delta), omega(omega) {
        dt = timestep;
    }
    /**
    * \brief Yields the derivative of the state vector (used by RK45)
    */
    vec2 f(const vec2 &y0, const double& t) const {
        return vec2({
            y0[1],
            -delta*y0[1] -alpha*y0[0] -beta*y0[0]*y0[0]*y0[0] + gamma*cos(omega*t)
        });
    }
    vec2 step(const vec2 &state) const override {
        // Use RK45 to integrate the system scheme
        cm::RK45<vec2,DuffingOscillator> rk45(this, 1e-8);
        return rk45.step(state, 0, dt, dt/50.0);
    }

    void setGamma(double gamma) {
        DuffingOscillator::gamma = gamma;
    }

};

#endif //CELL_MAPPING_CPP_DUFFING_H