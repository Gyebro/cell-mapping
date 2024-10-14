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

    /**
     *
     * @param red red channel as floating point value [0, 1]
     * @param green green channel as floating point value [0, 1]
     * @param blue blue channel as floating point value [0, 1]
     * @param hue output hue in [0, 1] range
     * @param saturation output saturation
     * @param value output value
     */
    void rgb2hsv(double red, double green, double blue, double &hue, double &saturation, double &value) {
        double max_ = fmax(fmax(red, green), fmax(red, blue));
        double min_ = fmin(fmin(red, green), fmin(red, blue));
        hue = saturation = value = max_;
        double delta = max_ - min_;
        if (max_ != 0) {
            saturation = delta / max_;
        } else {
            saturation = 0;
            hue = -1;
            return; // If value is 0, it's a grayscale image, hue is undefined.
        }
        if (red == max_) {
            hue = (green - blue) / delta;
        } else if (green == max_) {
            hue = 2.0 + (blue - red) / delta;
        } else {
            hue = 4.0 + (red - green) / delta;
        }
        hue *= 60.0;
        if (hue < 0) {
            hue += 360.0;
        }
        hue /= 360.0;
    }

}
