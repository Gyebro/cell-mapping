#ifndef CELL_MAPPING_CPP_COLORING_H
#define CELL_MAPPING_CPP_COLORING_H

#include <vector>
#include <cmath>
#include "cell.h"

namespace cm {

    void hsv2rgb(double h, double s, double v, double& r, double& g, double& b);

    template <class CellType, class IDType>
    class SCMColoringMethod {
    public:
        virtual std::vector<char> createColor(const CellType& cell, const IDType periodicGroups) {
            return std::vector<char>(3, 0);
        }
    };

    template <class CellType, class IDType>
    class SCMDefaultColoring : public SCMColoringMethod<CellType, IDType> {
        std::vector<char> createColor(const CellType& cell,
                                      const IDType periodicGroups) {
            IDType group = cell.getGroup();
            IDType step = cell.getStep();
            // Create a HSV color with constant saturation
            double h, s, v;
            h = double(group) / periodicGroups;
            s = 0.8; // Constant saturation
            double transient_steps = 500.0;
            if (step > 0) { /* Transient with shading */
                v = 0.85 - fmin(0.5 * (double(step) / transient_steps), 0.5);
            } else { // Periodic cells will be white
                v=1.0; s=0.0;
            }
            if (group == 0) { // Sink cell's domain is white
                v=1.0; s=0.0;
            }
            double r, g, b;
            hsv2rgb(h, s, v, r, g, b);
            std::vector<char> rgb(3);
            rgb[0]=(char)(r*255.0);
            rgb[1]=(char)(g*255.0);
            rgb[2]=(char)(b*255.0);
            return rgb;
        }
    };

    template <class CellType, class IDType>
    class ClusteredSCMDefaultColoring : public SCMColoringMethod<CellType, IDType> {
        std::vector<char> createColor(const CellType& cell,
                                      const IDType periodicGroups) {
            IDType group = cell.getGroup();
            IDType step = cell.getStep();
            IDType clusterID = cell.getClusterID();
            // Create a HSV color with constant saturation
            double h, s, v;
            h = double(group) / periodicGroups;
            s = 0.8;
            double transient_steps = 500.0;
            if (step > 0) { /* Transient with shading */
                v = 0.85 - fmin(0.5 * (double(step) / transient_steps), 0.5);
            } else { // Periodic cells will be white
                v=1.0; s=0.0;
            }
            if (group == 0) { // Sink cell's domain is white
                v=1.0; s=0.0;
            }
            double r, g, b;
            hsv2rgb(h, s, v, r, g, b);
            std::vector<char> rgb(3);
            rgb[0]=(char)(r*255.0);
            rgb[1]=(char)(g*255.0);
            rgb[2]=(char)(b*255.0);
            return rgb;
        }
    };

    template <class CellType, class IDType>
    class SCMHeatMapColoring : public SCMColoringMethod<CellType, IDType> {
        std::vector<char> createColor(const CellType& cell,
                                      const IDType periodicGroups) {
            IDType group = cell.getGroup();
            IDType step = cell.getStep();
            // Create a HSV color
            double h, s, v;
            double transient_steps = 200.0;
            h = double(step)/transient_steps; // Hue based on step number
            s = 0.8; // Constant saturation
            v = 0.8; // Constant value
            if (group == 0) {
                s = 0.0;
                v = 1; // Sink cell's domain
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

    template <class CellType, class IDType>
    class SCMBlackAndWhiteColoring : public SCMColoringMethod<CellType, IDType> {
        std::vector<char> createColor(const CellType& cell,
                                      const IDType periodicGroups) {
            IDType group = cell.getGroup();
            IDType step = cell.getStep();
            // Create a HSV color
            double h, s, v;
            h = 0.0;
            s = 0.0; // Constant saturation
            v = 0.2+0.8*double(step)/300.0; if (v > 1.0) v = 1.0;
            if (step == 0) { v = 0.0; } // Periodic cells will be black
            double r, g, b;
            hsv2rgb(h, s, v, r, g, b);
            std::vector<char> rgb(3);
            rgb[0]=(char)(r*255);
            rgb[1]=(char)(g*255);
            rgb[2]=(char)(b*255);
            return rgb;
        }
    };
}


#endif //CELL_MAPPING_CPP_COLORING_H