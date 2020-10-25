#pragma once

#include "math.h"
#include "ray.h"

typedef struct sphere {
    point3 center;
    float radius;
    color color;
} sphere;

typedef struct hit_record {
    point3 point;
    hmm_v3 normal;
    float t;
    bool front_face;
} hit_record;

void fill_hit_record(hit_record* rec, const float t, const ray* r, const sphere* s) {
    rec->t = t;
    rec->point = ray_at(r, rec->t);
    rec->normal = HMM_DivideVec3f(HMM_SubtractVec3(rec->point, s->center), s->radius);
    rec->front_face = HMM_DotVec3(r->direction, rec->normal) < 0.f;

    if (!rec->front_face) {
        rec->normal.X = -rec->normal.X;
        rec->normal.Y = -rec->normal.Y;
        rec->normal.Z = -rec->normal.Z;
    }
}

bool hit_sphere(const sphere* s, const ray* r, float t_min, float t_max, hit_record* rec) {
    const hmm_v3 oc = HMM_SubtractVec3(r->origin, s->center);
    const float a = HMM_LengthSquaredVec3(r->direction);
    const float half_b = HMM_DotVec3(oc, r->direction);
    const float c = HMM_LengthSquaredVec3(oc) - s->radius * s->radius;
    const float discriminant = half_b * half_b - a * c;

    if (discriminant > 0.f) {
        const float root = sqrtf(discriminant);

        float t = (-half_b - root) / a;
        if (t < t_max && t > t_min) {
            fill_hit_record(rec, t, r, s);
            return true;
        }

        t = (-half_b + root) / a;
        if (t < t_max && t > t_min) {
            fill_hit_record(rec, t, r, s);
            return true;
        }
    }

    return false;
}