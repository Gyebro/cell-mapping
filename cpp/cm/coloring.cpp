//
// Created by Gyebro on 2019-02-27.
//

#include "coloring.h"

namespace cm {

    void hsv2rgb(double h, double s, double v, double& r, double& g, double& b) {
        h *= 6.0; // Span to 360 deg.
        double c = v * s;
        double x = c * (1.0 - fabs(fmod(h, 2.0)-1.0));
        r = g = b = 0.0;
        if( h < 1.0f ) {
            r = c;
            g = x;
        } else if ( h < 2.0f ) {
            g = c;
            r = x;
        } else if ( h < 3.0f ) {
            g = c;
            b = x;
        } else if ( h < 4.0f ) {
            b = c;
            g = x;
        } else if ( h < 5.0f ) {
            b = c;
            r = x;
        } else {
            r = c;
            b = x;
        }
        r += v-c;
        g += v-c;
        b += v-c;
    }

}
