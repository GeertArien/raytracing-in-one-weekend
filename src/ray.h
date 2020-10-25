#pragma once

#include "math.h"

typedef struct ray {
    point3 origin;
    hmm_v3 direction;
} ray;

point3 ray_at(const ray* r, const float t) {
    const hmm_v3 offset = HMM_MultiplyVec3f(r->direction, t);
    return HMM_AddVec3(r->origin, offset);
};
