#include "lozi.h"

vec2 LoziMap::step(const vec2 &state) const  {
    double xn = state[0];
    double yn = state[1];
//    double tn = 0.4-6.0/(1.0+xn*xn+yn*yn);
    //double tn = 4.7-37.6/(1.0+xn*xn+yn*yn);
    vec2 ynp1;
    ynp1[0] = 1.0 - a*fabs(xn) + yn;
    ynp1[1] = b*xn;
    return ynp1;
}