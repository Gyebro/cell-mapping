#include "cmlib.h"
#include "jerk_saturated_acc.h"
#include "gnuplot_i.h"

using namespace cm;
using namespace std;

int main() {

    bool single_plot = false;

    double dT = 0.1;
    double t_fol = 2.4;
    double m = -0.4;
    double tau = 0.2;
    double v_avg = 15;
    double j_max = 5;
    double j_min = -5;
    double k = 100;
    Jerk_staruated_acc limitCycle(dT, t_fol, m, tau, v_avg, j_max, j_min, k);

    vec3 center = {v_avg*t_fol, v_avg, 0.0};
    vec3 width  = {30, 30, 30};
    vector<uint32_t> cells = {600, 600, 300};

    if (single_plot) {
        SCM<SCMCell<uint32_t>, uint32_t, vec3> scm(center, width, cells, &limitCycle);
        scm.solve(6);
        if (scm.getPeriodicGroups() > 1) {
            scm.exportGroups("groups.txt", 3);
            std::vector<vec3> pg = scm.getAllPGs();
            Gnuplot gp;
            gp.cmd("set terminal wxt size 800,700 enhanced font 'CMU Serif,14'");
            std::vector<double> ptsX;
            std::vector<double> ptsY;
            std::vector<double> ptsZ;
            for (const vec3& v : pg) {
                ptsX.push_back(v[0]); ptsY.push_back(v[1]); ptsZ.push_back(v[2]);
            }
            gp.plot_xyz(ptsX, ptsY, ptsZ, "All limit cycles and fixed points");
            // Allow user to interact with gnuplot
            std::cout << "Press [ENTER] to exit" << std::endl;
            getchar();
        } else {
            std::cout << "No periodic groups were found, exiting...\n";
        }
    } else {
        // "Bifurcation diagram"
        double tau_min = 0.05;
        double tau_max = 1;
        size_t steps = 50;
        std::vector<double> ptsT, ptsXmax, ptsYmax, ptsZmax;
        std::vector<double> ptsXmin, ptsYmin, ptsZmin;
        for (double tau_var = tau_min; tau_var <= tau_max; tau_var += (tau_max-tau_min)/steps) {
            limitCycle.setTau(tau_var);
            std::cout << "Running SCM with tau = " << tau_var << std::endl;
            SCM<SCMCell<uint32_t>, uint32_t, vec3> scm(center, width, cells, &limitCycle);
            scm.solve(6);
            if (scm.getPeriodicGroups() > 1) {
                ptsT.push_back(tau_var);
                std::vector<vec3> pg = scm.getAllPGs();
                double xmax = pg[0][0];
                double ymax = pg[0][1];
                double zmax = pg[0][2];
                double xmin = xmax;
                double ymin = ymax;
                double zmin = zmax;
                for (vec3 v : pg) {
                    if (v[0] > xmax) xmax = v[0];
                    if (v[1] > ymax) ymax = v[1];
                    if (v[2] > zmax) zmax = v[2];
                    if (v[0] < xmin) xmin = v[0];
                    if (v[1] < ymin) ymin = v[1];
                    if (v[2] < zmin) zmin = v[2];
                }
                ptsXmax.push_back(xmax); ptsXmin.push_back(xmin);
                ptsYmax.push_back(ymax); ptsYmin.push_back(ymin);
                ptsZmax.push_back(zmax); ptsZmin.push_back(zmin);
            }
        }
        Gnuplot gp;
        gp.cmd("set terminal wxt size 800,700 enhanced font 'CMU Serif,14'");
        gp.plot_xy(ptsT,ptsXmax,"X_{max}");
        gp.plot_xy(ptsT,ptsXmin,"X_{min}");
        gp.plot_xy(ptsT,ptsYmax,"Y_{max}");
        gp.plot_xy(ptsT,ptsYmin,"Y_{min}");
        gp.plot_xy(ptsT,ptsZmax,"Z_{max}");
        gp.plot_xy(ptsT,ptsZmin,"Z_{min}");
        // Allow user to interact with gnuplot
        std::cout << "Press [ENTER] (twice) to exit" << std::endl;
        getchar();
    }

    return 0;
}