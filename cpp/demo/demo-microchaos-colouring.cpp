#include <iostream>

#include "cmlib.h"
#include "microchaos.h"
#include "vec3.h"

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
    vector<pair<double, vec3>> gradient;
public:
    MyColoringMethod() {
        /* Sea colours */
        gradient.push_back(make_pair<double, vec3>(0, {188.0/360.0, 0.0, 1.0}));
        gradient.push_back(make_pair<double, vec3>(10.0, {162.0/360.0, 40.0/100.0, 80.0/100.0}));
        gradient.push_back(make_pair<double, vec3>(60.0, {188.0/360.0, 40.0/100.0, 90.0/100.0}));
        gradient.push_back(make_pair<double, vec3>(100.0, {177.0/360.0, 60.0/100.0, 70.0/100.0}));
        gradient.push_back(make_pair<double, vec3>(150.0, {196.0/360.0, 70.0/100.0, 70.0/100.0}));
        gradient.push_back(make_pair<double, vec3>(200.0, {196.0/360.0, 70.0/100.0, 50.0/100.0}));
    }
    std::vector<char> createColor(const CellType& cell,
                                  const IDType periodicGroups) {
        IDType group = cell.getGroup();
        IDType step = cell.getStep();
        double st = static_cast<double>(step);
        // Create a HSV color
        vec3 hsv;
        if (group == 0) {
            hsv = gradient.back().second;
        } else {
            hsv = gradient.back().second;
            for (size_t i=0; i<gradient.size(); i++) {
                if (st < gradient[i].first) { // i always larger than zero, because gradient always starts at zero
                    double t = (st-gradient[i-1].first)/(gradient[i].first-gradient[i-1].first);
                    hsv = gradient[i-1].second * (1.0-t) + gradient[i].second * t;
                    break;
                }
            }
        }
        double r, g, b;
        hsv2rgb(hsv[0], hsv[1], hsv[2], r, g, b);
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
    uint32_t mult = 1;
    uint32_t margin_h = mult * 3000;
    uint32_t margin_v = mult * 3000;
    uint32_t target_w = mult * 4000;
    uint32_t target_h = mult * 3000;
    vec2 width  = {2500.0 * (target_w + 2 * margin_h) / (target_w),
                   96.0 * (target_h + 2 * margin_v) / (target_h)};
    vector<uint32_t> cells = {target_w + 2 * margin_h, target_h + 2 * margin_v};

    MyColoringMethod<SCMCell<uint32_t>, uint32_t> coloringMethod;
    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &system);
    scm.solve(20);

    scm.generateImage("scm-microchaos-colouring.jpg", &coloringMethod, margin_h, margin_v, cells[0] - 2 * margin_h, cells[1] - 2 * margin_v);

    return 0;
}