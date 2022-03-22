#include <iostream>
#include <fstream>
#include "cmlib.h"
#include "microchaos.h"


using namespace cm;
using namespace std;

void minimize(vec2& min, const vec2 v) {
    if (v[0] < min[0]) min[0] = v[0];
    if (v[1] < min[1]) min[1] = v[1];
}

void maximize(vec2& max, const vec2 v) {
    if (v[0] > max[0]) max[0] = v[0];
    if (v[1] > max[1]) max[1] = v[1];
}

int main() {

    double P = 0.0573;
    double D = 0.53;
    double alpha = 0.00066;
    double delta = 0;


    vec2 center = {0, 0};
    vec2 width  = {50, 3};
    uint32_t cell_scale_x = 30;
    uint32_t cell_scale_y = 8*60;
    vector<uint32_t> cells = {static_cast<uint32_t>(width[0])*cell_scale_x, static_cast<uint32_t>(width[1])*cell_scale_y};

    vec2 min = {1000,1000};
    vec2 max = {-1000,-1000};

    ofstream log("scm-microchaos-rin-variation-3.txt");
    char sep = '\t';

    log << "P" << sep << "W_x" << sep << "W_y" << std::endl;
    P = 0.0573;
    //P = 0.01;
    for (double p = 0.0001; p<=P; p+=0.00005) {
        MicroChaosMapStaticRoundIn system(p, D, alpha, delta);
        SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &system);
        scm.solve();
        auto css = scm.getCss();
        for (size_t i=1; i<css.getCellSum(); i++) {
            if (css.getCell(i).getGroup() > 0 && css.getCell(i).getStep() == 0) {
                vec2 cnt = css.getCenter(i);
                minimize(min, cnt);
                maximize(max, cnt);
            }
        }
        scm.generateImage("scm-microchaos-rin_3_"+to_string(p)+".jpg");
        std::cout << "P = " << p << ", |x| = " << fabs(max[0]-min[0]) << ", |y| = " << fabs(max[1]-min[1])  << std::endl;
        log << p << sep << fabs(max[0]-min[0]) << sep << fabs(max[1]-min[1]) << std::endl;
    }
    log.close();


    //

    return 0;
}