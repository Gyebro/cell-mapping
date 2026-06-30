#include <iostream>
#include <chrono>

#include "cmlib.h"
#include "sgms_stribeck.h"

using namespace cm;
using namespace std;
using namespace std::chrono;

typedef ICMCell<uint32_t, vec2> CType;

int main() {
    auto start = high_resolution_clock::now();
    SGMSStribeckModel system;

    vec2 center = {1.0, -3.0};
    vec2 width  = {4.0, 8.0};
    vector<uint32_t> cells = {1000, 1000};
    //vector<uint32_t> cells = {200, 200};

    ICM<CType, uint32_t, vec2> icm(center, width, cells, &system);
    SCMDefaultColoring<CType, uint32_t> coloringMethod0;
    SCMBlackAndWhiteColoring<CType, uint32_t> coloringMethod1;
    SCMHeatMapColoring<CType, uint32_t> coloringMethod2;
    icm.solve(1000, 0.03, 0.06);
    auto stop = high_resolution_clock::now();
    auto duration_s = duration_cast<seconds>(stop - start);
    auto duration_ms = duration_cast<milliseconds>(stop - start);
    std::cout << "Execution time: " << duration_s.count() << " s (" << duration_ms.count() << " ms)" << endl;
    icm.generateImage("icm-sgms-stribeck_"+to_string(cells[0])+"_default.jpg", &coloringMethod0);
    icm.generateImage("icm-sgms-stribeck_"+to_string(cells[0])+"_bw.jpg", &coloringMethod1);
    icm.generateImage("icm-sgms-stribeck_"+to_string(cells[0])+"_heat.jpg", &coloringMethod2);

    return 0;
}