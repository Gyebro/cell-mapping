#include <iostream>

#include "cmlib.h"
#include "microchaos.h"

using namespace cm;
using namespace std;

/**
 * This class implements a 5-step colour scale, sea colours
 * @tparam CellType
 * @tparam IDType
 */
template <class CellType, class IDType>
class MyColoringMethod : public SCMColoringMethod<CellType, IDType> {
private:
    /* Sea colours */
    /*double hue=199.0/360.0;
    double s5=100.0/100.0, v5=20.0/100.0;
    double s4=100.0/100.0, v4=32.0/100.0;
    double s3= 69.0/100.0, v3=43.0/100.0;
    double s2= 66.0/100.0, v2=47.0/100.0;
    double s1= 12.0/100.0, v1=85.0/100.0;
    double s0= 0.0, v0=1.0;*/
    double h5=196.0/360.0, s5= 70.0/100.0, v5=50.0/100.0;
    double h4=196.0/360.0, s4= 70.0/100.0, v4=70.0/100.0;
    double h3=177.0/360.0, s3= 60.0/100.0, v3=70.0/100.0;
    double h1=162.0/360.0, s1= 40.0/100.0, v1=80.0/100.0;
    double h2=188.0/360.0, s2= 40.0/100.0, v2=90.0/100.0;
    double h0=188.0/360.0, s0= 0.0, v0=1.0;
public:
    std::vector<char> createColor(const CellType& cell,
                                  const IDType periodicGroups) {
        IDType group = cell.getGroup();
        IDType step = cell.getStep();
        double st = static_cast<double>(step);
        // Create a HSV color
        double h, s, v;
        // Step breakpoints for 4kx2k cells
        double t1 = 10; // White -> Light blue
        double t2 = 60; // Lt. blue -> Blue
        double t3 = 100;
        double t4 = 150;
        double t5 = 200;
        if (group == 0) {
            h = h5;
            s = s5;
            v = v5;
        } else {
            if (st < t1) {
                double t = (st)/t1;
                s = s0*(1.0-t) + s1*t;
                v = v0*(1.0-t) + v1*t;
                h = h0*(1.0-t) + h1*t;
            } else if (st < t2) {
                double t = (st-t1)/(t2-t1);
                s = s1*(1.0-t) + s2*t;
                v = v1*(1.0-t) + v2*t;
                h = h1*(1.0-t) + h2*t;
            } else if (st < t3) {
                double t = (st-t2)/(t3-t2);
                s = s2*(1.0-t) + s3*t;
                v = v2*(1.0-t) + v3*t;
                h = h2*(1.0-t) + h3*t;
            } else if (st < t4) {
                double t = (st-t3)/(t4-t3);
                s = s3*(1.0-t) + s4*t;
                v = v3*(1.0-t) + v4*t;
                h = h3*(1.0-t) + h4*t;
            } else if (st < t5) {
                double t = (st-t4)/(t4-t3);
                s = s4*(1.0-t) + s5*t;
                v = v4*(1.0-t) + v5*t;
                h = h4*(1.0-t) + h5*t;
            } else {
                s = s5;
                v = v5;
                h = h5;
            }
        }
        double r, g, b;
        hsv2rgb(h, s, v, r, g, b);
        std::vector<char> rgb(3);
        rgb[0]=(char)(r*255);
        rgb[1]=(char)(g*255);
        rgb[2]=(char)(b*255);
        return rgb;
    }
};

int main() {

    double P = 0.007;
    double D = 0.02;
    double alpha = 0.078;
    double delta = 0;
    MicroChaosMapStatic system(P, D, alpha, delta);

    vec2 center = {0, 0};
    vec2 width  = {2500, 70};
    vector<uint32_t> cells = {4000, 2000};//{8000, 4000};

    MyColoringMethod<SCMCell<uint32_t>, uint32_t> coloringMethod;
    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &system);
    scm.solve(20);
    scm.generateImage("scm-microchaos-colouring.jpg", &coloringMethod);

    return 0;
}