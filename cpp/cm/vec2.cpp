#include "vec2.h"
#include <algorithm>
#include <iterator>

vec2::vec2() {
    std::fill(std::begin(data), std::end(data), 0);
}

vec2::vec2(std::initializer_list<double> l) {
    std::copy_n(l.begin(), 2, std::begin(data));
}

double& vec2::operator[] (unsigned int i) {
    return data[i];
}

const double& vec2::operator[] (unsigned int i) const {
    return data[i];
}

double& mat2::operator[](unsigned int i) {
    return data[i];
}

const double& mat2::operator[](unsigned int i) const {
    return data[i];
}

double& mat2::operator()(unsigned int i, unsigned int j) {
    return data[i*2+j];
}

const double& mat2::operator()(unsigned int i, unsigned int j) const {
    return data[i*2+j];
}

// Arithmetic operations on vec2 and mat2

double operator*(const vec2& l, const vec2& r) {
    return l[0]*r[0]+l[1]*r[1];
}

vec2 operator*(const mat2& L, const vec2& r) {
    return vec2({
                        L(0,0)*r[0] + L(0,1)*r[1],
                        L(1,0)*r[0] + L(1,1)*r[1]
                });
}

vec2 operator*(const vec2& v, const double d) {
    return vec2({v[0]*d, v[1]*d});
}

vec2 operator*(const double d, const vec2& v) {
    return vec2({v[0]*d, v[1]*d});
}

vec2 operator+(const vec2& l, const vec2& r) {
    return vec2({l[0]+r[0], l[1]+r[1]});
}

vec2 operator-(const vec2& l, const vec2& r) {
    return vec2({l[0]-r[0], l[1]-r[1]});
}

double norm(const vec2 &v) {
    return sqrt(v[0]*v[0]+v[1]*v[1]);
}
