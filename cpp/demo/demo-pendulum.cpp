#include "cmlib.h"
#include "pendulum.h"

using namespace cm;
using namespace std;

int main() {

    double alpha = 1.0;
    double delta = 0.2;
    double integration_time = 0.1;
    Pendulum pendulum(alpha, delta, integration_time);

    vec2 center = {0.0, 0.0};
    vec2 width  = {16.0*M_PI, 10.0};
    vector<uint32_t> cells = {1400, 800};

    SCM32<vec2> scm(center, width, cells, &pendulum);
    scm.solve(20);
    scm.generateImage("pendulum.jpg");

    return 0;
}
