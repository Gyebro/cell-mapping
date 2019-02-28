#ifndef CELL_MAPPING_CPP_VEC2_H
#define CELL_MAPPING_CPP_VEC2_H

#include <initializer_list>
#include <cmath>

/**
 * Very simple 2-element vector class
 */
class vec2 {
private:
    double data[2]; /// Container of components
public:
    /**
     * Default constructor, fills the vector with zeros
     */
    vec2();
    /**
     * Constructor with initializer_lists.
     * Example:
     * vec2 v = {0, 1}
     */
    vec2(std::initializer_list<double> l);
    /**
     * Operator[] for accessing the i-th component (mutable)
     */
    double& operator[] (unsigned int i);
    /**
     * Operator[] for accessing the i-th component (const)
     */
    const double& operator[] (unsigned int i) const;
};

/**
 * Very simple 2x2 matrix class
 */
class mat2 {
private:
    double data[4]; /// Container of components, row-major
public:
    /**
     * Operator[] for accessing the i-th component (mutable)
     */
    double& operator[](unsigned int i);
    /**
     * Operator[] for accessing the i-th component (const)
     */
    const double& operator[](unsigned int i) const;
    /**
     * Operator() for accessing the (i,j)-th component (mutable)
     * @param i row index
     * @param j column index
     */
    double& operator()(unsigned int i, unsigned int j);
    /**
     * Operator() for accessing the (i,j)-th component (const)
     * @param i row index
     * @param j column index
     */
    const double& operator()(unsigned int i, unsigned int j) const;
};

/**
 * Vector scalar product
 * @param l
 * @param r
 * @return the scalar product of l*r
 */
double operator*(const vec2& l, const vec2& r);

/**
 * Matrix-vector multiplication
 * @param L square matrix
 * @param r column vector
 * @return the vector L*r
 */
vec2 operator*(const mat2& L, const vec2& r);

/**
 * Vector-scalar multiplication (scaling)
 * @param v vector
 * @param d scalar
 * @return the vector v*d (=d*v)
 */
vec2 operator*(const vec2& v, const double d);

/**
 * Vector-scalar multiplication (scaling)
 * @param d scalar
 * @param v vector
 * @return the vector d*v (=v*d)
 */
vec2 operator*(const double d, const vec2& v);

/**
 * Vector addition
 * @param l
 * @param r
 * @return l+r (=r+l)
 */
vec2 operator+(const vec2& l, const vec2& r);

/**
 * Vector subtraction
 * @param l minuend
 * @param r subtrahend
 * @return l-r
 */
vec2 operator-(const vec2& l, const vec2& r);

/**
 * Vector L2 norm
 * @param v
 * @return square root of the sum of vector components' squares.
 */
double norm(const vec2& v);

#endif //CELL_MAPPING_CPP_VEC2_H