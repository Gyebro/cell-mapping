#ifndef CELL_MAPPING_SGMS_STRIBECK_H
#define CELL_MAPPING_SGMS_STRIBECK_H

#include "system.h"
#include "vec2.h"
#include "rk45.h"

template<class T>
T sgn(T a) {
    if (a >= 0) {return T(1.0);}
    else {return T(-1.0);}
}

/**
 * \brief SGMS-Stribeck model
 * Using y = [v, z]
 * v' = k*z - C*v + Fe
 * z' = -v * sgn(1 + k*z*sgn(v)/Ff(v))
 * where
 * Ff(v) = Fd + (Fs-Fd)*exp(-(abs(v)/vS)^delta)
 * (Note: v = y[0], z = y[1])
 */
class SGMSStribeckModel : public cm::DynamicalSystemBase<vec2>  {
private:
    double k;
    double C;
    double Fe;
    double Fd;
    double Fs;
    double vS;
    double delta;
    double dt;
public:
    SGMSStribeckModel(double k=0.2,
        double c=0.5,
        double fe=1.065,
        double fd=0.1,
        double fs=1.0,
        double vs=1.0,
        double delta=2.0,
        double dt=0.05)
        : k(k),
          C(c),
          Fe(fe),
          Fd(fd),
          Fs(fs),
          vS(vs),
          delta(delta),
          dt(dt) {
    }
    inline double Ff(const double& v) const {
        return Fd + (Fs-Fd)*exp(-pow(abs(v)/vS,delta));
    }
    /**
    * \brief Yields the derivative of the state vector (used by RK45)
    */
    vec2 f(const vec2 &y0, const double& t) const {
        return vec2({
            k*y0[1] - C*y0[0] + Fe,
            -y0[0] * sgn(1.0 + k*y0[1]*sgn(y0[0])/Ff(y0[0]))
        });
    }
    vec2 step(const vec2 &state) const override {
        // Use RK45 to integrate the system scheme
        cm::RK45<vec2,SGMSStribeckModel> rk45(this, 1e-6);
        return rk45.step(state, 0, dt, dt/50.0);
    }

};

#endif //CELL_MAPPING_SGMS_STRIBECK_H