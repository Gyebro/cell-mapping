#include "ikeda.h"

vec2 IkedaMap::step(const vec2 &state) const  {
    double xn = state[0];
    double yn = state[1];
    double tn = 0.4-6.0/(1.0+xn*xn+yn*yn);
    //double tn = 4.7-37.6/(1.0+xn*xn+yn*yn);
    vec2 ynp1;
    ynp1[0] = 1.0 + u*(xn * cos(tn) - yn * sin(tn));
    ynp1[1] = u*(xn * sin(tn) + yn * cos(tn));
    //ynp1[0] = 2.16 + u*(xn * cos(tn) - yn * sin(tn));
    //ynp1[1] = u*(xn * sin(tn) + yn * cos(tn));
    return ynp1;
}