#include "vec3.h"
#include <algorithm>
#include <iterator>

vec3::vec3() {
    std::fill(std::begin(data), std::end(data), 0);
}

vec3::vec3(std::initializer_list<double> l) {
    std::copy_n(l.begin(), 3, std::begin(data));
}

double& vec3::operator[] (unsigned int i) {
    return data[i];
}

const double& vec3::operator[] (unsigned int i) const {
    return data[i];
}

vec3 operator*(const vec3& v, const double d) {
    return vec3({v[0]*d, v[1]*d, v[2]*d});
}

vec3 operator*(const double d, const vec3& v) {
    return vec3({v[0]*d, v[1]*d, v[2]*d});
}

vec3 operator+(const vec3& l, const vec3& r) {
    return vec3({l[0]+r[0], l[1]+r[1], l[2]+r[2]});
}

vec3 operator-(const vec3& l, const vec3& r) {
    return vec3({l[0]-r[0], l[1]-r[1], l[2]-r[2]});
}

double norm(const vec3 &v) {
    return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}