#include "cmlib.h"
#include "rolling.h"
#include "gnuplot_i.h"

using namespace cm;
using namespace std;

int main() {

    double dt = 0.005;
    RollingStepan limitCycle(dt);

    vec3 center = {0.0, 0.0, 0.0};
    vec3 width  = {0.6, 6, 0.22};
    vector<uint32_t> cells = {300, 300, 300}; //{600, 300, 600};

    SCMBinaryColoring<SCMCell<uint32_t>, uint32_t> coloringMethod;
    bool single_sim = false;

    Gnuplot gp;
    gp.cmd("set terminal wxt size 800,700 enhanced font 'CMU Serif,14'");
    std::vector<double> ptsX;
    std::vector<double> ptsY;
    std::vector<double> ptsZ;

    if (single_sim) {
        vec3 s = {-0.24, 0.4, 0};
        for (size_t i=0; i<100; i++) {
            ptsX.push_back(s[0]);
            ptsY.push_back(s[1]);
            ptsZ.push_back(s[2]);
            s = limitCycle.step(s);
        }
    } else {
        SCM<SCMCell<uint32_t>, uint32_t, vec3> scm(center, width, cells, &limitCycle);
        scm.solve(1);
        //scm.exportGroups("groups.txt", 3);
        std::vector<vec3> pg = scm.getAllPGs();
        for (const vec3& v : pg) {
            ptsX.push_back(v[0]); ptsY.push_back(v[1]); ptsZ.push_back(v[2]);
        }
    }

    gp.plot_xyz(ptsX, ptsY, ptsZ, "Rolling system (Stepan 1991)");
    std::cout << "Press [ENTER] to exit" << std::endl;
    getchar();

    return 0;
}
