#include "cmlib.h"
#include "duffing.h"

using namespace cm;
using namespace std;

int main() {

    double alpha = -1.0;
    double beta  = 1.0;
    double gamma = 0.28;
    double delta = 0.3;
    double omega = 1.2;
    double integration_time = 2*M_PI/omega;
    DuffingOscillator duffing(alpha, beta, gamma, delta, omega, integration_time);

    vec2 center = {0.0, 0.0};
    vec2 width  = {4.0, 3.0};
    vector<uint32_t> cells = {1000, 1000};

    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &duffing);
    SCMBlackAndWhiteColoring<SCMCell<uint32_t>, uint32_t> coloringMethod;

    vector<double> gammas = {0.28, 0.29, 0.37, 0.5};

    for (double g : gammas) {
        duffing.setGamma(g);
        scm.solve(20);
        scm.generateImage("scm-duffing-gamma="+to_string(g)+".jpg", &coloringMethod);
    }

    return 0;
}
