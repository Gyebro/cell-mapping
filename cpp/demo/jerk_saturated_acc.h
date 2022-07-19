#ifndef CELL_MAPPING_CPP_JERK_SATURATED_ACC_H
#define CELL_MAPPING_CPP_JERK_SATURATED_ACC_H

#include "system.h"
#include "vec3.h"
#include "rk45.h"

class Jerk_staruated_acc : public cm::DynamicalSystemBase<vec3> {
private:
    double dT;
    double t_fol;
    double m;
    double tau;
    double v_avg;
    double j_max;
    double j_min;
    double k;
public:
    Jerk_staruated_acc(double timestep, double t_fol, double m, double tau,
                        double v_avg, double j_max, double j_min, double k):
                        dT(dT), t_fol(t_fol), m(m), tau(tau),
                        v_avg(v_avg), j_max(j_max), j_min(j_min), k(k) {
                            dT = timestep;
    }
    void setTau(const double Tau) {
        tau = Tau;
    }
    vec3 step(const vec3 &y0) const override {
        // Jerk saturated acc
        double dx = -y0[2]/2*dT*dT + y0[0] + v_avg*dT - y0[1]*dT;
        double v_ego = y0[2]*dT + y0[1];
        // Heaviside functions of the a_control
        double mid_func = (((v_avg - y0[1])-m*(y0[0]-v_avg*t_fol))/tau)/2;
        double min_boundary = (((v_avg - y0[1])-m*(y0[0]-v_avg*t_fol))/tau-y0[2])/dT-j_min;
        double max_boundary = (((v_avg - y0[1])-m*(y0[0]-v_avg*t_fol))/tau-y0[2])/dT-j_max;
        double H_mid = mid_func*(1+tanh(k*min_boundary))-mid_func*(1+tanh(k*max_boundary));
        double min_func = (y0[2]+j_min*dT)/2;
        double H_min = min_func*(1+tanh(-k*min_boundary));
        double max_func = (y0[2]+j_max*dT)/2;
        double H_max = max_func*(1+tanh(k*max_boundary));
        double a_control = H_mid+H_min+H_max;
        return vec3({
            dx,
            v_ego,
            a_control
        });
    }
};
#endif //CELL_MAPPING_CPP_JERK_SATURATED_ACC_H