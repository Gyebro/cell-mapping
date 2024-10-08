#include "microchaos.h"
#include <cmath>

MicroChaosMapStatic::MicroChaosMapStatic(double P, double D, double alpha, double delta) {
    MicroChaosMapStatic::alpha = alpha;
    MicroChaosMapStatic::delta = delta;
    MicroChaosMapStatic::P = P;
    MicroChaosMapStatic::D = D;
    Gamma = sqrt(1.0+delta*delta);
    U = Us(1.0);
    b = bs(1.0);
    k[0] = P;
    k[1] = D;
}

mat2 MicroChaosMapStatic::Us(double s) const {
    mat2 Uss;
    double chags = cosh(alpha*Gamma*s);
    double shags = sinh(alpha*Gamma*s);
    double eads = exp(alpha*delta*s);
    double emads = 1.0/eads;
    Uss(0,0) = (delta*shags+Gamma*chags)*emads/Gamma;
    Uss(0,1) = (shags/alpha)*emads/Gamma;
    Uss(1,0) = (alpha*shags)*emads/Gamma;
    Uss(1,1) = (Gamma*chags-delta*shags)*emads/Gamma;
    return Uss;
}

vec2 MicroChaosMapStatic::bs(double s) const {
    vec2 bss;
    double chags = cosh(alpha*Gamma*s);
    double shags = sinh(alpha*Gamma*s);
    double eads = exp(alpha*delta*s);
    double emads = 1.0/eads;
    bss[0]   = (Gamma-emads*(Gamma*chags+delta*shags))/(Gamma*alpha*alpha);
    bss[1]   = -shags*emads/(alpha*Gamma);
    return bss;
}

vec2 MicroChaosMapStatic::step(const vec2& y0) const {
    double M;
    M = SymmetricFloor(k*y0); // Rounding at output
    vec2 y1 = U*y0 + b*M;
    return y1;
}

MicroChaosMapEmbeddedStatic::MicroChaosMapEmbeddedStatic(double P, double D, double alpha, double delta) {
    MicroChaosMapEmbeddedStatic::alpha = alpha;
    MicroChaosMapEmbeddedStatic::delta = delta;
    MicroChaosMapEmbeddedStatic::P = P;
    MicroChaosMapEmbeddedStatic::D = D;
    Gamma = sqrt(1.0+delta*delta);
    U = Us(1.0);
    b = bs(1.0);
    k[0] = P; // control gain for position
    k[1] = 0; // control gain is zero for "true" velocity, not directly measured
    k[2] = D; // control gain for estimated velocity
}

mat2 MicroChaosMapEmbeddedStatic::Us(double s) const {
    mat2 Uss;
    double chags = cosh(alpha*Gamma*s);
    double shags = sinh(alpha*Gamma*s);
    double eads = exp(alpha*delta*s);
    double emads = 1.0/eads;
    Uss(0,0) = (delta*shags+Gamma*chags)*emads/Gamma;
    Uss(0,1) = (shags/alpha)*emads/Gamma;
    Uss(1,0) = (alpha*shags)*emads/Gamma;
    Uss(1,1) = (Gamma*chags-delta*shags)*emads/Gamma;
    return Uss;
}

vec2 MicroChaosMapEmbeddedStatic::bs(double s) const {
    vec2 bss;
    double chags = cosh(alpha*Gamma*s);
    double shags = sinh(alpha*Gamma*s);
    double eads = exp(alpha*delta*s);
    double emads = 1.0/eads;
    bss[0]   = (Gamma-emads*(Gamma*chags+delta*shags))/(Gamma*alpha*alpha);
    bss[1]   = -shags*emads/(alpha*Gamma);
    return bss;
}

vec3 MicroChaosMapEmbeddedStatic::step(const vec3& y0) const {
    double M;
    M = SymmetricFloor(k*y0); // Rounding at output using estimated velocity = (pos-prev_pos)
    vec2 y0s = {y0[0], y0[1]};
    vec2 y1s = U*y0s + b*M; // The evolution of the system due to it's internal unstable behaviour (U*y) + constant M torque.
    vec3 y1 = {y1s[0], y1s[1], y1s[0]-y0[0]};
    return y1;
}


MicroChaosMapStaticRoundIn::MicroChaosMapStaticRoundIn(double P, double D, double alpha, double delta) {
    MicroChaosMapStaticRoundIn::alpha = alpha;
    MicroChaosMapStaticRoundIn::delta = delta;
    MicroChaosMapStaticRoundIn::P = P;
    MicroChaosMapStaticRoundIn::D = D;
    Gamma = sqrt(1.0+delta*delta);
    U = Us(1.0);
    b = bs(1.0);
    k[0] = P;
    k[1] = D;
}

mat2 MicroChaosMapStaticRoundIn::Us(double s) const {
    mat2 Uss;
    double chags = cosh(alpha*Gamma*s);
    double shags = sinh(alpha*Gamma*s);
    double eads = exp(alpha*delta*s);
    double emads = 1.0/eads;
    Uss(0,0) = (delta*shags+Gamma*chags)*emads/Gamma;
    Uss(0,1) = (shags/alpha)*emads/Gamma;
    Uss(1,0) = (alpha*shags)*emads/Gamma;
    Uss(1,1) = (Gamma*chags-delta*shags)*emads/Gamma;
    return Uss;
}

vec2 MicroChaosMapStaticRoundIn::bs(double s) const {
    vec2 bss;
    double chags = cosh(alpha*Gamma*s);
    double shags = sinh(alpha*Gamma*s);
    double eads = exp(alpha*delta*s);
    double emads = 1.0/eads;
    bss[0]   = (Gamma-emads*(Gamma*chags+delta*shags))/(Gamma*alpha*alpha);
    bss[1]   = -shags*emads/(alpha*Gamma);
    return bss;
}

vec2 MicroChaosMapStaticRoundIn::step(const vec2& y0) const {
    double M;
    vec2 y0r = {SymmetricFloor(y0[0]), SymmetricFloor(y0[1])};
    M = k*y0r; // Rounding at input
    vec2 y1 = U*y0 + b*M;
    return y1;
}