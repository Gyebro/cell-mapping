#ifndef CELL_MAPPING_CPP_PENDULUM_H
#define CELL_MAPPING_CPP_PENDULUM_H

#include "system.h"
#include "vec2.h"

/**
 * \brief Ikeda map
 * x_(n+1) = 1 + u (x_n cos(t_n) - y_n sin(t_n))
 * y_(n+1) = u (x_n sin(t_n) + y_n cos(t_n))
 */
class IkedaMap : public cm::DynamicalSystemBase<vec2>  {
private:
    double u;
public:
    IkedaMap(double par_u) : u(par_u) {

    }
    vec2 step(const vec2 &state) const override;
    void setU(const double par_u) {
        u = par_u;
    }

};

#endif //CELL_MAPPING_CPP_PENDULUM_H