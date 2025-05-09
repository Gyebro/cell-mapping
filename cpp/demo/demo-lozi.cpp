#include <iostream>

#include "cmlib.h"
#include "lozi.h"

using namespace cm;
using namespace std;

int main() {

    LoziMap system;

    vec2 center = {0, 0};
    vec2 width  = {18.0, 18.0};
    vector<uint32_t> cells = {1000, 1000};

    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &system);
    SCMHeatMapColoring<SCMCell<uint32_t>, uint32_t> coloringMethod0;
    scm.solve(1);
    scm.generateImage("scm-lozi.jpg", &coloringMethod0);

    return 0;
}