#include "cmlib.h"
#include "ikeda.h"

using namespace cm;
using namespace std;

int main() {

    double u = 0.96;

    IkedaMap map(u);

    vec2 center = {0.0, 0.0};
    vec2 width  = {6.0, 6.0};
    vector<uint32_t> cells = {600, 600};
    vec2 center2 = center - vec2({width[0], 0});
    vec2 center3 = center2 + vec2({0, width[1]});
    vec2 center4 = center3 + vec2({0, width[1]});
    vec2 center5 = center4 + vec2({width[0], 0});
    vec2 center6 = center5 + vec2({width[0], 0});
    vec2 center7 = center6 + vec2({width[0], 0});
    vec2 center8 = center7 - vec2({0, width[1]});
    vec2 center9 = center8 - vec2({0, width[1]});
    vec2 center10 = center9 - vec2({width[0], 0});
    vec2 center11 = center10 + vec2({0, width[1]});
    vec2 center12 = center + vec2({0, width[1]});

    /*BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm0(center, width, cells, &map);
    scm0.solve(20);
    scm0.generateImage("step_0_scm_ikeda_blocked_");

    BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm1(center, width, cells, &map);

    scm1.addBlock(center2, width, cells);
    scm1.solve(20);
    scm1.generateImage("step_1_scm_ikeda_blocked_");

    BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm2(center, width, cells, &map);

    scm2.addBlock(center2, width, cells);
    scm2.addBlock(center3, width, cells);
    scm2.solve(20);
    scm2.generateImage("step_2_scm_ikeda_blocked_");

    BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm3(center, width, cells, &map);

    scm3.addBlock(center2, width, cells);
    scm3.addBlock(center3, width, cells);
    scm3.addBlock(center4, width, cells);
    scm3.solve(20);
    scm3.generateImage("step_3_scm_ikeda_blocked_");

    BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm4(center, width, cells, &map);

    scm4.addBlock(center2, width, cells);
    scm4.addBlock(center3, width, cells);
    scm4.addBlock(center4, width, cells);
    scm4.addBlock(center5, width, cells);
    scm4.solve(20);
    scm4.generateImage("step_4_scm_ikeda_blocked_");

    BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm5(center, width, cells, &map);

    scm5.addBlock(center2, width, cells);
    scm5.addBlock(center3, width, cells);
    scm5.addBlock(center4, width, cells);
    scm5.addBlock(center5, width, cells);
    scm5.addBlock(center6, width, cells);
    scm5.solve(20);
    scm5.generateImage("step_5_scm_ikeda_blocked_");

    BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm6(center, width, cells, &map);

    scm6.addBlock(center2, width, cells);
    scm6.addBlock(center3, width, cells);
    scm6.addBlock(center4, width, cells);
    scm6.addBlock(center5, width, cells);
    scm6.addBlock(center6, width, cells);
    scm6.addBlock(center7, width, cells);
    scm6.solve(20);
    scm6.generateImage("step_6_scm_ikeda_blocked_");

    BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm7(center, width, cells, &map);

    scm7.addBlock(center2, width, cells);
    scm7.addBlock(center3, width, cells);
    scm7.addBlock(center4, width, cells);
    scm7.addBlock(center5, width, cells);
    scm7.addBlock(center6, width, cells);
    scm7.addBlock(center7, width, cells);
    scm7.addBlock(center8, width, cells);
    scm7.solve(20);
    scm7.generateImage("step_7_scm_ikeda_blocked_");*/

    SCMHeatMapColoring<SCMCell<uint32_t>, uint32_t> coloringMethod0;
    BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm11(center, width, cells, &map);
    scm11.addBlock(center2, width, cells);
    scm11.addBlock(center3, width, cells);
    scm11.addBlock(center4, width, cells);
    scm11.addBlock(center5, width, cells);
    scm11.addBlock(center6, width, cells);
    scm11.addBlock(center7, width, cells);
    scm11.addBlock(center8, width, cells);
    scm11.addBlock(center9, width, cells);
    scm11.addBlock(center10, width, cells);
    scm11.addBlock(center11, width, cells);
    scm11.solve(20);
    scm11.generateImage("step_11_scm_ikeda_blocked_" , &coloringMethod0, true);

    SCMHeatMapColoring<SCMCell<uint32_t>, uint32_t> coloringMethod;
    BSCM<SCMCell<uint32_t>, uint32_t, vec2> scm12(center, width, cells, &map);
    scm12.addBlock(center2, width, cells);
    scm12.addBlock(center3, width, cells);
    scm12.addBlock(center4, width, cells);
    scm12.addBlock(center5, width, cells);
    scm12.addBlock(center6, width, cells);
    scm12.addBlock(center7, width, cells);
    scm12.addBlock(center8, width, cells);
    scm12.addBlock(center9, width, cells);
    scm12.addBlock(center10, width, cells);
    scm12.addBlock(center11, width, cells);
    scm12.addBlock(center12, width, cells);
    scm12.solve(20);
    scm12.generateImage("step_12_scm_ikeda_blocked_", &coloringMethod, false);




    return 0;

/*  SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &map);
    SCMBlackAndWhiteColoring<SCMCell<uint32_t>, uint32_t> coloringMethod;

    //map.setU(u);
    //scm.solve(20);
    //scm.generateImage("scm-ikeda-series-u="+to_string(u)+".jpg",  &coloringMethod);

    center = {0.0, 0.0};
    width  = {6.0, 6.0};
    cells = {600, 600};
    SCM<ClusterableSCMCell<uint32_t>, uint32_t, vec2> scm1(center, width, cells, &map);
    scm1.solve(20);
    scm1.generateImage("scm-ikeda_1_initial.jpg");

    vec2 center2 = center - vec2({width[0], 0});
    SCM<ClusterableSCMCell<uint32_t>, uint32_t, vec2> scm2(center2, width, cells, &map);
    scm2.solve(20);
    scm2.generateImage("scm-ikeda_2_initial.jpg");

    ClusteredSCM<ClusterableSCMCell<uint32_t>, uint32_t, vec2> cscm(&scm1, &scm2);
    bool verbose = true; // This will generate output images after the joining procedure
    cscm.join(verbose);
*/


    /*for (double u=0.5; u<0.7; u+=0.025) {
        map.setU(u);
        scm.solve(20);
        scm.generateImage("scm-ikeda-bounds-u="+to_string(u)+".jpg", &coloringMethod);
    }*/

    /*u = 1.8;
    map.setU(u);
    scm.solve(20);
    //scm.generateImage("scm-ikeda-u="+to_string(u)+".jpg", &coloringMethod);
    scm.generateImage("scm-ikeda-u="+to_string(u)+".jpg");*/

    return 0;
}
