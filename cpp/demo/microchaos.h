#ifndef CELL_MAPPING_CPP_MICROCHAOS_H
#define CELL_MAPPING_CPP_MICROCHAOS_H

#include "system.h"
#include "vec2.h"

class MicroChaosMapStatic : public cm::DynamicalSystemBase<vec2> {
protected:
    double alpha;	/**< Natural frequency */
    double delta;	/**< Relative damping */
    double P;		/**< Proportional control gain */
    double D;		/**< Derivative control gain */
    double Gamma;	/**< System parameter Gamma=sqrt(1+delta^2) */
    mat2 U;			/**< System matrix U */
    vec2 b;			/**< External force vector b */
    vec2 k;			/**< Control vector k */
    /**
     * \brief Calculates U(s) for a given dimensionless time
     */
    mat2 Us(double s) const;
    /**
     * \brief Calculates b(s) for a given dimensionless time
     */
    vec2 bs(double s) const;
public:
    /**
     * \brief Initializes the system with its parameters
     */
    MicroChaosMapStatic(double P, double D, double alpha, double delta);
    /**
    * \brief Step with the micro-chaos map
    */
    vec2 step(const vec2 &y0) const;
};

class MicroChaosMapStaticRoundIn : public cm::DynamicalSystemBase<vec2> {
protected:
    double alpha;	/**< Natural frequency */
    double delta;	/**< Relative damping */
    double P;		/**< Proportional control gain */
    double D;		/**< Derivative control gain */
    double Gamma;	/**< System parameter Gamma=sqrt(1+delta^2) */
    mat2 U;			/**< System matrix U */
    vec2 b;			/**< External force vector b */
    vec2 k;			/**< Control vector k */
    /**
     * \brief Calculates U(s) for a given dimensionless time
     */
    mat2 Us(double s) const;
    /**
     * \brief Calculates b(s) for a given dimensionless time
     */
    vec2 bs(double s) const;
public:
    /**
     * \brief Initializes the system with its parameters
     */
    MicroChaosMapStaticRoundIn(double P, double D, double alpha, double delta);
    /**
    * \brief Step with the micro-chaos map
    */
    vec2 step(const vec2 &y0) const;
};

template<class T>
T SymmetricFloor(T a) {
    if (a >= 0) {return floor(a);}
    else {return -floor(abs(a));}
}

template<class T>
T Sign(T a) {
    if (a >= 0) {return T(1.0);}
    else {return T(-1.0);}
}

#endif //CELL_MAPPING_CPP_MICROCHAOS_H