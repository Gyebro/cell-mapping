//
// Created by Gyebro on 2019-02-26.
//

#include "cmlib.h"
#include "duffing.h"

using namespace cm;
using namespace std;

int main() {

    double alpha = -1.0;
    double beta  = 1.0;
    double gamma = 0.5;
    double delta = 0.3;
    double omega = 1.2;
    double timestep = 2*M_PI/omega/20.0;
    DuffingOscillator duffing(alpha, beta, gamma, delta, omega, timestep);

    vec2 center = {0.0, 0.0};
    vec2 width  = {4.0, 3.0};
    vector<uint32_t> cells = {2000, 2000};

    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &duffing);
    scm.solve(20);
    scm.generateImage("scm-duffing.jpg");


    return 0;
}
