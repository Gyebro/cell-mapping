#ifndef CELL_MAPPING_CPP_VEC3_H
#define CELL_MAPPING_CPP_VEC3_H

#include <initializer_list>
#include <cmath>

/**
 * Very simple 3-element vector class
 */
class vec3 {
private:
    double data[3]; /// Container of components
public:
    /**
     * Default constructor, fills the vector with zeros
     */
    vec3();
    /**
     * Constructor with initializer_lists.
     * Example:
     * vec2 v = {0, 1, -2}
     */
    vec3(std::initializer_list<double> l);
    /**
     * Operator[] for accessing the i-th component (mutable)
     */
    double& operator[] (unsigned int i);
    /**
     * Operator[] for accessing the i-th component (const)
     */
    const double& operator[] (unsigned int i) const;
};


vec3 operator*(const vec3& v, const double d);

vec3 operator*(const double d, const vec3& v);

vec3 operator+(const vec3& l, const vec3& r);

vec3 operator-(const vec3& l, const vec3& r);

double norm(const vec3& v);

#endif //CELL_MAPPING_CPP_VEC3_H
