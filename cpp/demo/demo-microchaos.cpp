#include <iostream>

#include "cmlib.h"
#include "microchaos.h"

using namespace cm;
using namespace std;

#define DEMO_ROUNDING_AT_INPUT

int main() {

#ifdef DEMO_ROUNDING_AT_INPUT
    double P = 1.00011; // P_best for alpha = 0.007
    double D = 1.0;
    double alpha = 0.007;
    double delta = 0;

    P = 1.6;
    D = 1.50076;
    alpha = 0.2;
    delta = 0;

    P = 0.2;
    D = 0.2;
    alpha = 0.1;
    delta = 0;
    MicroChaosMapStatic system(P, D, alpha, delta, RoundingType::In);

    vec2 center = {0, 0};
    vec2 width  = {10, 4};
    uint32_t s=1;
    vector<uint32_t> cells = {s*1000, s*400};
#else
    double P = 0.007;
    double D = 0.02;
    double alpha = 0.078;
    double delta = 0;
    MicroChaosMapStatic system(P, D, alpha, delta, RoundingType::Out);

    vec2 center = {0, 0};
    vec2 width  = {2400, 50};
    vector<uint32_t> cells = {1000, 400};
#endif


    SCMBlackAndWhiteColoring<SCMCell<uint32_t>, uint32_t> coloringMethod;
    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &system);
    scm.solve(20);
    scm.printSummary();
    scm.generateImage("scm-microchaos.jpg", &coloringMethod);

    return 0;
}