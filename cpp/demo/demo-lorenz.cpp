#include "cmlib.h"
#include "lorenz.h"
#include "gnuplot_i.h"

using namespace cm;
using namespace std;

int main() {

    double dt = 0.025;
    LimitCycle limitCycle(dt);

    vec3 center = {0.0, 0.0, 0.0};
    vec3 width  = {220.0, 8.0, 6.0};
    vector<uint32_t> cells = {1200, 600, 1200}; //{600, 300, 600};

    SCMBinaryColoring<SCMCell<uint32_t>, uint32_t> coloringMethod;

    SCM<SCMCell<uint32_t>, uint32_t, vec3> scm(center, width, cells, &limitCycle);
    scm.solve(6);
    //scm.exportGroups("groups.txt", 3);
    std::vector<vec3> pg = scm.getPG(1);
    Gnuplot gp;
    gp.cmd("set terminal wxt size 800,700 enhanced font 'CMU Serif,14'");
    std::vector<double> ptsX;
    std::vector<double> ptsY;
    std::vector<double> ptsZ;
    for (const vec3& v : pg) {
        ptsX.push_back(v[0]); ptsY.push_back(v[1]); ptsZ.push_back(v[2]);
    }
    gp.plot_xyz(ptsX, ptsY, ptsZ, "Two-tube limit cycle");
    std::cout << "Press [ENTER] to exit" << std::endl;
    getchar();

    return 0;
}
