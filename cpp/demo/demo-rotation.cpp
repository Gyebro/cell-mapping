#include <iostream>

#include "cmlib.h"

#include "system.h"
#include "vec2.h"

using namespace cm;
using namespace std;

class RotationMap : public cm::DynamicalSystemBase<vec2> {
protected:
    double a;	    /**< Rotation angle */
    mat2 A;			/**< System matrix A */
    /**
     * \brief Calculates A for a given alpha
     */
    mat2 Rmx(double a) const {
		mat2 R;
		R(0,0) = cos(a);
		R(0,1) = -sin(a);
		R(1,0) = sin(a);
		R(1,1) = cos(a);
		return R;
	}
public:
    /**
     * \brief Initializes the system with its parameters
     */
    RotationMap(double alpha) : a(alpha), A(Rmx(a)) {
	}
    /**
    * \brief Step with the rotation map
    */
    vec2 step(const vec2 &y0) const {
		vec2 y1 = A*y0;
		return y1;
	}
};

int main() {

    double alpha = M_PI/5.5; // 0.05 vs 0.5
    cout << alpha << endl;
    RotationMap system(alpha);

    vec2 center = {0, 0};
    vec2 width  = {1000, 1000};
    vector<uint32_t> cells = {1000, 1000};

    SCM<SCMCell<uint32_t>, uint32_t, vec2> scm(center, width, cells, &system);
    scm.solve(20);
    scm.generateImage("scm-rotation.jpg");

    return 0;
}