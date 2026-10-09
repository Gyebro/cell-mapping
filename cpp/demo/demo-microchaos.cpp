#include <iostream>
#include <chrono>

#include "cmlib.h"
#include "../systems/microchaos.h"

using namespace cm;
using namespace std;
using namespace std::chrono;

typedef SCMCell<uint32_t> SCM_t;
typedef ICMCell<uint32_t, vec2> ICM_t;

int main() {

    /*double P = 0.009;
    double D = 0.02;
    double alpha = 0.078;
    double delta = 0;
    MicroChaosMapStatic system(P, D, alpha, delta);

    vec2 center = {0, 0};
    vec2 width  = {1200, 50};
    vector<uint32_t> cells = {1000, 500};*/

    double P = 0.009;
    double D = 0.02;
    double alpha = 0.078;
    double delta = 0;
    MicroChaosMapStatic system(P, D, alpha, delta);

    vec2 center = {150, 0};
    vec2 width  = {280, 30};
    //vector<uint32_t> cells = {1200, 500};
    vector<uint32_t> cells = {600, 250};

    SCM<SCM_t, uint32_t, vec2> scm(center, width, cells, &system);
    scm.solve(20);
    scm.generateImage("scm-microchaos_"+to_string(cells[0])+".jpg");

    vector<uint32_t> cells_2x = {2*cells[0], 2*cells[1]};
    SCM<SCM_t, uint32_t, vec2> scm_2x(center, width, cells_2x, &system);
    scm_2x.solve(20);
    SCMDefaultColoring<SCM_t, uint32_t> sColoringMethod0;
    SCMBlackAndWhiteColoring<ICM_t, uint32_t> sColoringMethod1;
    SCMHeatMapColoring<ICM_t, uint32_t> sColoringMethod2;
    scm_2x.generateImage("scm-microchaos_"+to_string(cells[0])+"_x2.jpg");
    scm_2x.generateImage("scm-microchaos_"+to_string(cells[0])+"_x2.jpg");
    scm_2x.generateImage("scm-microchaos_"+to_string(cells[0])+"_x2.jpg");

    auto start = high_resolution_clock::now();
    ICM<ICM_t, uint32_t, vec2> icm(center, width, cells, &system);
    SCMDefaultColoring<ICM_t, uint32_t> coloringMethod0;
    SCMBlackAndWhiteColoring<ICM_t, uint32_t> coloringMethod1;
    SCMHeatMapColoring<ICM_t, uint32_t> coloringMethod2;
    icm.build_graph(1000, 0.05, 0.1);
    icm.solve();
    auto stop = high_resolution_clock::now();
    auto duration_s = duration_cast<seconds>(stop - start);
    auto duration_ms = duration_cast<milliseconds>(stop - start);
    std::cout << "Execution time (first pass): " << duration_s.count() << " s (" << duration_ms.count() << " ms)" << endl;

    icm.generateImage2("icm-microchaos_"+to_string(cells[0])+"_default_firstpass.jpg", &coloringMethod0);
    start = high_resolution_clock::now();
    icm.fill_graph(1000, 0.05, 0.1);
    icm.solve();
    stop = high_resolution_clock::now();
    duration_s = duration_cast<seconds>(stop - start); duration_ms = duration_cast<milliseconds>(stop - start);
    std::cout << "Execution time (fill): " << duration_s.count() << " s (" << duration_ms.count() << " ms)" << endl;

    icm.generateImage2("icm-microchaos_"+to_string(cells[0])+"_default_filled.jpg", &coloringMethod0);
    icm.generateImage2("icm-microchaos_"+to_string(cells[0])+"_bw_filled.jpg", &coloringMethod1);
    icm.generateImage2("icm-microchaos_"+to_string(cells[0])+"_heat_filled.jpg", &coloringMethod2);

    return 0;
}