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
        gradient.push_back(make_pair<double, vec3>(250.0, {196.0/360.0, 70.0/100.0, 40.0/100.0}));
        gradient.push_back(make_pair<double, vec3>(400.0, {196.0/360.0, 70.0/100.0, 30.0/100.0}));
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

vec3 rgb2hsv(vec3 rgb_int) {
    double h, s, v;
    rgb2hsv(rgb_int[0]/255.0, rgb_int[1]/255.0, rgb_int[2]/255.0, h, s, v);
    return vec3{h,s,v};
}

template <class CellType, class IDType>
class MyColoringMethod2 : public SCMColoringMethod<CellType, IDType> {
private:
    vector<pair<double, vec3>> gradient;
public:
    MyColoringMethod2() {
        /* Sea colours using RGB inputs */
        gradient.push_back(make_pair<double, vec3>(0, {188.0/360.0, 0.0, 1.0}));
        gradient.push_back(make_pair<double, vec3>(10.0,  rgb2hsv({122, 204, 180})));
        gradient.push_back(make_pair<double, vec3>(60.0,  rgb2hsv({138, 217, 229})));
        gradient.push_back(make_pair<double, vec3>(100.0, rgb2hsv({71, 178, 173})));
        gradient.push_back(make_pair<double, vec3>(150.0, rgb2hsv({54, 145, 178})));
        gradient.push_back(make_pair<double, vec3>(200.0, rgb2hsv({38, 104, 128})));
        gradient.push_back(make_pair<double, vec3>(250.0, rgb2hsv({31, 83, 102})));
        gradient.push_back(make_pair<double, vec3>(400.0, rgb2hsv({23, 62, 76})));
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
    uint32_t margin_h = mult * 3000 * 0;
    uint32_t margin_v = mult * 3000 * 0;
    uint32_t target_w = mult * 4000;
    uint32_t target_h = mult * 3000;
    vec2 width  = {2500.0 * (target_w + 2 * margin_h) / (target_w),
                   96.0 * (target_h + 2 * margin_v) / (target_h)};
    vector<uint32_t> cells = {target_w + 2 * margin_h, target_h + 2 * margin_v};

    MyColoringMethod<SCMCell<uint32_t>, uint32_t> coloringMethod;
    MyColoringMethod2<SCMCell<uint32_t>, uint32_t> coloringMethod2;
    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &system);
    scm.solve(20);

    //scm.generateImage("scm-microchaos-colouring_a.jpg", &coloringMethod, margin_h, margin_v, cells[0] - 2 * margin_h, cells[1] - 2 * margin_v);
    scm.generateImage("scm-microchaos-colouring_b.jpg", &coloringMethod2, margin_h, margin_v, cells[0] - 2 * margin_h, cells[1] - 2 * margin_v);

    return 0;
}