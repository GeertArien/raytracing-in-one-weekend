#pragma once

#include <stdlib.h>

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_NO_SSE
#include "hmm/HandmadeMath.h"

// Type aliases for vec3
typedef hmm_v3 point3;   // 3D point
typedef hmm_v3 color;    // RGB color

float random_float() {
    // Returns a random real in [0,1).
    return (float) rand() / (RAND_MAX + 1.f);
}

float random_float_interval(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max-min) * random_float();
}

hmm_v3 random_v3() {
    return HMM_Vec3(random_float(), random_float(), random_float());
}

hmm_v3 random_v3_interval(float min, float max) {
    return HMM_Vec3(random_float_interval(min,max), random_float_interval(min,max), random_float_interval(min,max));
}

hmm_v3 random_v3_in_unit_sphere() {
    while (true) {
        const hmm_v3 p = random_v3_interval(-1.f,1.f);
        if (HMM_LengthSquaredVec3(p) >= 1.f) continue;
        return p;
    }
}