#include "cmlib.h"
#include "../systems/pendulum.h"

using namespace cm;
using namespace std;

typedef ICMCell<uint32_t, vec2> ICM_t;

#define SCM_ICM_COMPARISON

int main() {

    double alpha = 1.0;
    double delta = 0.2;
    double integration_time = 0.1;
    IkedaMap pendulum(alpha, delta, integration_time);

    vec2 center = {0.0, 0.0};
    vec2 width  = {10.0*M_PI, 8.0};
    vector<uint32_t> cells = {1000, 500};

    SCM32<vec2> scm(center, width, cells, &pendulum);
    scm.solve(20);
    scm.generateImage("scm-pendulum.jpg");

#ifdef SCM_ICM_COMPARISON
    ICM<ICM_t, uint32_t, vec2> icm(center, width, cells, &pendulum);
    SCMDefaultColoring<ICM_t, uint32_t> coloringMethod0;
    SCMBlackAndWhiteColoring<ICM_t, uint32_t> coloringMethod1;
    SCMHeatMapColoring<ICM_t, uint32_t> coloringMethod2;

    icm.build_graph(1000, 0.05, 0.1);
    icm.solve();
    icm.generateImage2("icm-pendulum"+to_string(cells[0])+"_default_firstpass.jpg", &coloringMethod0);
    icm.fill_graph(1000, 0.05, 0.1);
    icm.solve();

    icm.generateImage2("icm-pendulum"+to_string(cells[0])+"_default_filled.jpg", &coloringMethod0);
    icm.generateImage2("icm-pendulum"+to_string(cells[0])+"_bw_filled.jpg", &coloringMethod1);
    icm.generateImage2("icm-pendulum"+to_string(cells[0])+"_heat_filled.jpg", &coloringMethod2);
#endif

    return 0;
}
