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

hmm_v3 random_unit_vector() {
    return HMM_NormalizeVec3(random_v3_in_unit_sphere()); 
}

hmm_v3 random_in_hemisphere(const hmm_v3* normal) {
    const hmm_v3 in_unit_sphere = random_v3_in_unit_sphere();
    if (HMM_DotVec3(in_unit_sphere, *normal) > 0.f) { 
        // In the same hemisphere as the normal
        return in_unit_sphere;
    }
    else {
        return HMM_Vec3(-in_unit_sphere.X, -in_unit_sphere.Y, -in_unit_sphere.Z);
    }
}

hmm_v3 reflect_v3(const hmm_v3* d, const hmm_v3* n) {
    const float dot = HMM_DotVec3(*d, *n);
    const hmm_v3 projected = HMM_MultiplyVec3f(*n, 2.f * dot);
    return HMM_SubtractVec3(*d, projected);
}

hmm_v3 refract_v3(const hmm_v3* d, const hmm_v3* n, float etai_over_etat) {
    float cos_theta = HMM_DotVec3(HMM_Vec3(-d->X, -d->Y, -d->Z), *n);
    cos_theta = HMM_MIN(cos_theta, 1.f);
    hmm_v3 r_out_perp = HMM_MultiplyVec3f(*n, cos_theta);
    r_out_perp = HMM_AddVec3(*d, r_out_perp);
    r_out_perp =  HMM_MultiplyVec3f(r_out_perp, etai_over_etat);
    const float y = -HMM_SquareRootF(HMM_ABS(1.f - HMM_LengthSquaredVec3(r_out_perp)));
    const hmm_v3 r_out_parallel = HMM_MultiplyVec3f(*n, y);
    return HMM_AddVec3(r_out_perp, r_out_parallel);
}

bool near_zero_v3(const hmm_v3* vec) {
    // Return true if the vector is close to zero in all dimensions.
    const float s = 1e-8;
    return (HMM_ABS(vec->X) < s) && (HMM_ABS(vec->Y) < s) && (HMM_ABS(vec->Z) < s);
}
