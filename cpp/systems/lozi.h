//#ifndef CELL_MAPPING_CPP_PENDULUM_H
//#define CELL_MAPPING_CPP_PENDULUM_H
#ifndef CELL_MAPPING_CPP_LOZI_H
#define CELL_MAPPING_CPP_LOZI_H

#include "system.h"
#include "vec2.h"

/**
 * \brief Lozi map
 * x_(n+1) = 1 -a abs(x_n) + y_n
 * y_(n+1) = b x_n
 */
class LoziMap : public cm::DynamicalSystemBase<vec2>  {
private:
    double a;
    double b;
public:
    LoziMap(double par_a = 1.2, double par_b = -1) : a(par_a), b(par_b) {

    }
    vec2 step(const vec2 &state) const override;
    void setA(const double par_a) {
        a = par_a;
    }
    void setB(const double par_b) {
        b = par_b;
    }
};

#endif //CELL_MAPPING_CPP_PENDULUM_H