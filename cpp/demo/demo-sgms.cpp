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

    ICM<CType, uint32_t, vec2> icm(center, width, cells, &system);
    SCMDefaultColoring<CType, uint32_t> coloringMethod0;
    icm.solve(800);
    auto stop = high_resolution_clock::now();
    auto duration_s = duration_cast<seconds>(stop - start);
    auto duration_ms = duration_cast<milliseconds>(stop - start);
    std::cout << "Execution time: " << duration_s.count() << " s (" << duration_ms.count() << " ms)" << endl;
    icm.generateImage("icm-sgms-stribeck.jpg", &coloringMethod0);

    return 0;
}