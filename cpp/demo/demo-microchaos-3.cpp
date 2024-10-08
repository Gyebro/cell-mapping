#include <iostream>
#include <algorithm>

#include "cmlib.h"
#include "microchaos.h"
#include "../gnuplot_i/gnuplot_i.h"

using namespace cm;
using namespace std;

int main() {



    size_t config = 2;
    if (config == 0) {
        double P = 0.007;
        double D = 0.02;
        double alpha = 0.078;
        double delta = 0;
        MicroChaosMapEmbeddedStatic system(P, D, alpha, delta);
        SCMHeatMapColoring<SCMCell<uint32_t>, uint32_t> coloringMethod0;
        vec3 center = {0, 0, 0};
        vec3 width  = {2500, 50, 50};
        vector<uint32_t> cells = {1000, 400, 400};
        SCM<SCMCell<uint32_t>, uint32_t, vec3> scm(center, width, cells, &system);
        scm.solve(20);
        scm.generateImageProjection("scm-microchaos-3d.jpg", &coloringMethod0);
    } else if (config == 1) {
        double P = 0.007;
        double D = 0.02;
        double alpha = 0.07;
        double delta = 0.01;
        MicroChaosMapEmbeddedStatic system(P, D, alpha, delta);
        //SCMBlackAndWhiteColoring<SCMCell<uint32_t>, uint32_t> coloringMethod0;
        SCMHeatMapColoring<SCMCell<uint32_t>, uint32_t> coloringMethod0;
        vec3 center = {0, 0, 0};
        vec3 width  = {1500, 36, 36};
        vector<uint32_t> cells = {1000, 500, 1000};//{1000, 400, 400};
        SCM<SCMCell<uint32_t>, uint32_t, vec3> scm(center, width, cells, &system);
        scm.solve(1);
        for (size_t imgs = 500; imgs < cells[2]; imgs++) {
            size_t zlevel = imgs;
            string old_str = to_string(imgs);
            size_t n_pad = 4;
            scm.generateImageSlice("scm-microchaos-3d-"+ std::string(4 - std::min(n_pad, old_str.length()), '0') + old_str + ".jpg", &coloringMethod0, zlevel);
        }
    } else if (config == 2) {
        double P = 0.007;
        double D = 0.02;
        double alpha = 0.07;
        double delta = 0.00;
        MicroChaosMapEmbeddedStatic system(P, D, alpha, delta);
        //SCMBlackAndWhiteColoring<SCMCell<uint32_t>, uint32_t> coloringMethod0;
        SCMHeatMapColoring<SCMCell<uint32_t>, uint32_t> coloringMethod0;
        vec3 center = {144, 0, 0};
        //vec3 width  = {5, 2, 2};

        //vec3 center = {285.25, 0, 0};
        vec3 width  = {5, 2, 2};
        vector<uint32_t> cells = {1000, 500, 500};//{1000, 400, 400};
        SCM<SCMCell<uint32_t>, uint32_t, vec3> scm(center, width, cells, &system);
        scm.solve(1);

        std::vector<vec3> pg;
        Gnuplot gp;
        gp.cmd("set terminal wxt size 800,700 enhanced font 'CMU Serif,14'");
        gp.set_xlabel("x_i");
        gp.set_ylabel("x'_i");
        gp.set_zlabel("x_i-x_{i-1}");
        std::vector<double> ptsX;
        std::vector<double> ptsY;
        std::vector<double> ptsZ;
        pg = scm.getAllPGs();
        ptsX.resize(0); ptsY.resize(0); ptsZ.resize(0);
        for (const vec3& v : pg) {
            ptsX.push_back(v[0]); ptsY.push_back(v[1]); ptsZ.push_back(v[2]);
        }
        if (ptsX.size() > 1) {
            gp.plot_xyz(ptsX, ptsY, ptsZ, "1st chaotic attractor");
        } else {
            std::cout << "No points to draw, adjust settings\n";
        }
        getchar();
    }



    return 0;
}