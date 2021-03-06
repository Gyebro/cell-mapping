#include <iostream>

#include "cmlib.h"
#include "microchaos.h"

using namespace cm;
using namespace std;

int main() {

    double P = 0.007;
    double D = 0.02;
    double alpha = 0.078;
    double delta = 0;
    MicroChaosMapStatic system(P, D, alpha, delta);

    vec2 center = {0, 0};
    vec2 width  = {2400, 50};
    vector<uint32_t> cells = {1000, 400};

    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &system);
    scm.solve(20);
    scm.generateImage("scm-microchaos.jpg");

    return 0;
}