#include <iostream>

#include "cscmb.h"
#include "../systems/microchaos.h"

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
    size_t max_steps = 20;

    BSCMJpeg<SCMCell<uint32_t>, uint32_t, vec2> scm1(center, width, cells, &system);
    scm1.solve(max_steps);
    vec2 center2 = center + vec2({width[0], 0});
    scm1.addBlock(center2, width, cells);
    scm1.update(max_steps);
    scm1.generateImage("cb_v2_final.jpg");

    // Create a full-size SCM for validation
    cout << "\nValidation:\n";
    vec2 center3 = center + vec2({width[0]/2, 0});
    vec2 width3({width[0]*2, width[1]});
    vector<uint32_t> cells3 = {cells[0]*2, cells[1]};
    BSCMJpeg<SCMCell<uint32_t>, uint32_t, vec2> scm3(center3, width3, cells3, &system);
    scm3.solve(20);
    scm3.generateImage("cb_v2_validation.jpg");


    return 0;
}