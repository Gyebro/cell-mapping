#include <iostream>
#include <map>

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

vec3 hexcode2rgb(std::string hexcode) {
    return vec3{static_cast<double>(stoul(hexcode.substr(0, 2), nullptr, 16)),
                static_cast<double>(stoul(hexcode.substr(2, 2), nullptr, 16)),
                static_cast<double>(stoul(hexcode.substr(4, 2), nullptr, 16))
    };
}

template <class CellType, class IDType>
class GradientColouring : public SCMColoringMethod<CellType, IDType> {
private:
    vector<pair<double, vec3>> gradient;
public:
    GradientColouring() {
        /* Sea colours using RGB inputs */
        gradient.push_back(make_pair<double, vec3>(0, rgb2hsv({254, 255, 255})));
        gradient.push_back(make_pair<double, vec3>(10.0,  rgb2hsv({122, 204, 180})));
        gradient.push_back(make_pair<double, vec3>(60.0,  rgb2hsv({138, 217, 229})));
        gradient.push_back(make_pair<double, vec3>(100.0, rgb2hsv({71, 178, 173})));
        gradient.push_back(make_pair<double, vec3>(150.0, rgb2hsv({54, 145, 178})));
        gradient.push_back(make_pair<double, vec3>(200.0, rgb2hsv({38, 104, 128})));
        gradient.push_back(make_pair<double, vec3>(250.0, rgb2hsv({31, 83, 102})));
        gradient.push_back(make_pair<double, vec3>(400.0, rgb2hsv({23, 62, 76})));
    }
    explicit GradientColouring(const vector<pair<double, string>> gradient_hex) {
        for (auto& g : gradient_hex) {
            gradient.push_back(make_pair(g.first, rgb2hsv(hexcode2rgb(g.second))));
        }
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

template <class CellType, class IDType>
class StateAwareGradientColouring : public SCMColoringMethod<CellType, IDType> {
private:
    vector<pair<double, vec3>> gradient_base;
    vector<pair<double, vec3>> gradient_alt;
    vector<IDType> alt_groups;
public:
    StateAwareGradientColouring() {
    }
    explicit StateAwareGradientColouring(const vector<pair<double, string>> gradient_hex) {
        vector<pair<double, vec3>> gradient;
        for (auto& g : gradient_hex) {
            gradient.emplace_back(g.first, rgb2hsv(hexcode2rgb(g.second)));
        }
        gradient_base = gradient;
    }
    void add_alt(const vector<IDType> target_groups, const vector<pair<double, string>> gradient_hex) {
        vector<pair<double, vec3>> gradient;
        for (auto& g : gradient_hex) {
            gradient.emplace_back(g.first, rgb2hsv(hexcode2rgb(g.second)));
        }
        alt_groups = target_groups;
        gradient_alt = gradient;
    }
    std::vector<char> createColor(const CellType& cell,
                                  const IDType periodicGroups) {
        IDType group = cell.getGroup();
        IDType step = cell.getStep();
        double st = static_cast<double>(step);
        vector<pair<double, vec3>> gradient;
        if (std::count(alt_groups.begin(), alt_groups.end(),group) > 0) {
            gradient = gradient_alt;
        } else {
            gradient = gradient_base;
        }
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

    double P = 0.008;
    double D = 0.02;
    double alpha = 0.08;
    double delta = 0;
    MicroChaosMapStatic system(P, D, alpha, delta);

    vec2 center = {0, 0};
    uint32_t mult = 2;
    uint32_t enable_margin = 1;
    uint32_t margin_h = mult * 1500 * enable_margin;
    uint32_t margin_v = mult * 1500 * enable_margin;
    uint32_t target_w = mult * 2000;
    uint32_t target_h = mult * 1000;
    //vec2 width  = {2500.0 * (target_w + 2 * margin_h) / (target_w),
    //               72.0 * (target_h + 2 * margin_v) / (target_h)};
    vec2 width  = {0.6*2500.0 * (target_w + 2 * margin_h) / (target_w),
                   0.7*72.0 * (target_h + 2 * margin_v) / (target_h)};
    vector<uint32_t> cells = {target_w + 2 * margin_h, target_h + 2 * margin_v};

    vector<pair<double, string>> gradient_sea = {
            {0.0, "feffff"},
            {10.0, "7accb4"},
            {60.0, "8ad9e5"},
            {100.0, "47b2ad"},
            {150.0, "3691b2"},
            {200.0, "266880"},
            {250.0, "1f5366"},
            {400.0, "173e4c"}
    };
    vector<pair<double, string>> gradient_isles_0 = {
            {0.0, "521d18"},
            {10.0, "7b2e24"},
            {25.0, "d9a494"},
            {28.0, "ffe2d9"},
            {30.0, "e5ffff"},
            {100.0, "00b4b5"},
            {200.0, "00658d"},
            {300.0, "042769"},
            {310.0, "91b8ff"},
            {320.0, "042769"}
    };
    vector<pair<double, string>> gradient_isles = {
            {0.0, "8f785e"},
            {3.0, "8f785e"},
            {10.0, "c3a583"},
            {25.0, "d4bfa4"},
            {28.0, "fff1de"},
            {30.0, "e5ffff"},
            {100.0, "00b4b5"},
            {200.0, "00658d"},
            {300.0, "042769"},
            {310.0, "91b8ff"},
            {320.0, "042769"}
    };
    vector<pair<double, string>> lake_balaton = {
            {0.0, "146d5b"},
            {10.0, "146d5b"},
            {20.0, "35a483"},
            {25.0, "65bda4"},
            {50.0, "74cbba"},
            {90.0, "51c6c0"},
            {120.0, "1faca6"},
            {150.0, "43c3c2"},
            {180.0, "1aadb5"},
            {210.0, "008a9b"},
            {240.0, "3f5c64"}
    };
    //GradientColouring<SCMCell<uint32_t>, uint32_t> gradient1(gradient_sea);
    //GradientColouring<SCMCell<uint32_t>, uint32_t> gradient2(lake_balaton);

    vector<pair<double, string>> gradient_base = {
            {0.0, "faf2e1"},
            {10.0, "faf2e1"},
            {30.0, "f9c152"},
            {50.0, "86a368"},
            {100.0, "1a93a8"},
            {200.0, "066375"},
            {400.0, "043e4a"}
    };
    vector<pair<double, string>> gradient_left = {
            {0.0, "faf2e1"},
            {10.0, "faf2e1"},
            {30.0, "fb6e0e"},
            {50.0, "9c5344"},
            {51.0, "783b6e"},
            {100.0, "114969"},
            {200.0, "0a1f34"},
            {400.0, "050f1a"}
    };

    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &system);
    scm.solve(5);
    vector<uint32_t> leftGroups = scm.getLeftGroups();

    StateAwareGradientColouring<SCMCell<uint32_t>, uint32_t> gradient_asymmetric(gradient_base);
    gradient_asymmetric.add_alt(leftGroups, gradient_left);

    //scm.generateImage("scm-microchaos-colouring_gradient.jpg", &gradient1, margin_h, margin_v, cells[0] - 2 * margin_h, cells[1] - 2 * margin_v);
    scm.generateImage("scm-microchaos_gradient_asymmetric.jpg", &gradient_asymmetric, margin_h, margin_v, cells[0] - 2 * margin_h, cells[1] - 2 * margin_v, 2);

    return 0;
}