#include <iostream>

#include "cmlib.h"
#include "microchaos.h"

using namespace cm;
using namespace std;

int main() {

    double P = 0.007;
    double D = 0.02;
    double alpha = 0.07;
    double delta = 0;
    MicroChaosMapStatic system(P, D, alpha, delta);

    vec2 center = {185.4, 0};
    vec2 width  = {200, 25};
    vector<uint32_t> cells = {1000, 1000};

    SCM<ClusterableSCMCell<uint32_t>, uint32_t, vec2> scm1(center, width, cells, &system);
    scm1.solve(20);
    scm1.generateImage("scm1_st0.jpg");

    vec2 center2 = center + vec2({width[0], 0});
    SCM<ClusterableSCMCell<uint32_t>, uint32_t, vec2> scm2(center2, width, cells, &system);
    scm2.solve(20);
    scm2.generateImage("scm2_st0.jpg");

    ClusteredSCM<ClusterableSCMCell<uint32_t>, uint32_t, vec2> cscm(&scm1, &scm2);
    cscm.join(true);


    return 0;
}