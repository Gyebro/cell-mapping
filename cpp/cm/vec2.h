#ifndef CELL_MAPPING_CPP_VEC2_H
#define CELL_MAPPING_CPP_VEC2_H

#include <initializer_list>
#include <cmath>

/**
 * \class Very simple 2-element vector class
 */
class vec2 {
private:
    double data[2];
public:
    vec2();
    vec2(std::initializer_list<double> l);
    double& operator[] (unsigned int i);
    const double& operator[] (unsigned int i) const;
};

/**
 * \class Very simple 2x2 matrix class
 */
class mat2 {
private:
    double data[4];
public:
    double& operator[](unsigned int i);
    const double& operator[](unsigned int i) const;
    double& operator()(unsigned int i, unsigned int j);
    const double& operator()(unsigned int i, unsigned int j) const;
};

double operator*(const vec2& l, const vec2& r);

vec2 operator*(const mat2& L, const vec2& r);

vec2 operator*(const vec2& v, const double d);

vec2 operator*(const double d, const vec2& v);

vec2 operator+(const vec2& l, const vec2& r);

vec2 operator-(const vec2& l, const vec2& r);

double norm(const vec2& v);

#endif //CELL_MAPPING_CPP_VEC2_H