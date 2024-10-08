#include "cmlib.h"
#include "lorenz.h"
#include "gnuplot_i.h"

using namespace cm;
using namespace std;

int main() {

    double dt = 0.025*2;
    size_t max_steps = 1;
    LimitCycle limitCycle(dt);

    vec3 center = {0.0, 0.0, 0.0};
    vec3 width  = {220.0, 8.0, 6.0};
    vector<uint32_t> cells = {600, 300, 600};//{1200, 600, 1200}; //
    //cells = {300,150,300};

    bool single_sim = false;
    bool interactive = true;

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
        gp.plot_xyz(ptsX, ptsY, ptsZ, "Two-tube limit cycle");
        std::cout << "Press [ENTER] to exit" << std::endl;
        getchar();
    } else if (interactive) {
        SCM<SCMCell<uint32_t>, uint32_t, vec3> scm(center, width, cells, &limitCycle);
        scm.solve(max_steps);
        size_t step_num = 2;
        size_t max_step_num = 4;
        bool excl_sink = true;
        bool pgs = false;
        while (interactive) {
            if (step_num > scm.getCss().getCellSum()) {
                step_num = 0;
            }
            if (max_step_num > scm.getCss().getCellSum()) {
                max_step_num = 10;
            }
            if (max_step_num < step_num) {
                max_step_num = step_num+2;
            }
            std::cout << "Current step num: " << step_num << "-" << max_step_num << std::endl;
            std::cout << "i/I: increase\n";
            std::cout << "d/D: decrease\n";
            std::cout << "s: toggle sink DOA\n";
            std::cout << "p: toggle periodic groups\n";
            std::cout << "q: quit\n";
            char c;
            // redraw
            std::vector<vec3> pg;
            if (pgs) {
                pg = scm.getAllPGs();
            } else {
                pg = scm.getTransients(step_num, max_step_num, excl_sink);
            }
            ptsX.resize(0); ptsY.resize(0); ptsZ.resize(0);
            for (const vec3& v : pg) {
                ptsX.push_back(v[0]); ptsY.push_back(v[1]); ptsZ.push_back(v[2]);
            }
            gp.remove_tmpfiles();
            gp.reset_all();
            gp.cmd("set terminal wxt size 800,700 enhanced font 'CMU Serif,14'");
            if (ptsX.size() > 1) {
                gp.plot_xyz(ptsX, ptsY, ptsZ, "Two-tube limit cycle");
            } else {
                std::cout << "No points to draw, adjust settings\n";
            }
            std::cin >> c;
            switch (c) {
                case 'i':
                    step_num += 2;
                    break;
                    case 'd':
                        step_num -= 2;
                        break;
                        case 'I':
                            max_step_num += 2;
                            break;
                            case 'D':
                                max_step_num -= 2;
                                break;
                                case 's':
                                    excl_sink = !excl_sink;
                                    break;
                                    case 'p':
                                        pgs = !pgs;
                                        break;
                                        case 'q':
                                            interactive = false;
                                            break;
                                            default:
                                                break;
            }
        }
    } else {
        SCM<SCMCell<uint32_t>, uint32_t, vec3> scm(center, width, cells, &limitCycle);
        scm.solve(1);
        //scm.exportSolution("twotube_sol.txt", 3);
        std::vector<vec3> pg = scm.getAllPGs();
        for (const vec3& v : pg) {
            ptsX.push_back(v[0]); ptsY.push_back(v[1]); ptsZ.push_back(v[2]);
        }
        gp.plot_xyz(ptsX, ptsY, ptsZ, "Two-tube limit cycle");
        std::cout << "Press [ENTER] to exit" << std::endl;
        getchar();
    }

    return 0;
}
