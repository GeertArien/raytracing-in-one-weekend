#pragma once

#include "math.h"
#include "material.h"
#include "ray.h"

typedef struct sphere {
    point3 center;
    float radius;
    material material;
} sphere;

typedef struct hit_record {
    point3 point;
    hmm_v3 normal;
    float t;
    material material;
    bool front_face;
} hit_record;

bool scatter_ray(const material* mat, const ray* r_in, const hit_record* rec, color* attenuation, ray* scattered) {

    if (mat->reflect) {
        const hmm_v3 dir_n = HMM_NormalizeVec3(r_in->direction); 
        const hmm_v3 reflected = reflect_v3(&dir_n, &rec->normal);
        scattered->origin = rec->point;
        const hmm_v3 fuzz_vec = HMM_MultiplyVec3f(random_v3_in_unit_sphere(), mat->fuzz);
        scattered->direction = HMM_AddVec3(reflected, fuzz_vec);
        *attenuation = mat->albedo;
        return (HMM_DotVec3(scattered->direction, rec->normal) > 0.f);
    }

    if (mat->dielectric) {
        *attenuation = HMM_Vec3(1.f, 1.f, 1.f);
        float refraction_ratio = rec->front_face ? (1.f / mat->ir) : mat->ir;

        const hmm_v3 dir_n = HMM_NormalizeVec3(r_in->direction); 

        float cos_theta = HMM_DotVec3(HMM_Vec3(-dir_n.X, -dir_n.Y, -dir_n.Z), rec->normal);
        cos_theta = HMM_MIN(cos_theta, 1.f);
        const float sin_theta = HMM_SquareRootF(1.f - cos_theta * cos_theta);

        const bool cannot_refract = refraction_ratio * sin_theta > 1.f;
        hmm_v3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float()) {
            direction = reflect_v3(&dir_n, &rec->normal);
        }
        else {
            direction = refract_v3(&dir_n, &rec->normal, refraction_ratio);
        }

        scattered->origin = rec->point;
        scattered->direction = direction;
        return true;
    }

    hmm_v3 scatter_direction = HMM_AddVec3(rec->normal, random_unit_vector());
    //hmm_v3 scatter_direction = random_in_hemisphere(&rec->normal);

     // Catch degenerate scatter direction
    if (near_zero_v3(&scatter_direction)) {
        scatter_direction = rec->normal;
    }

    scattered->origin = rec->point;
    scattered->direction = scatter_direction;
    *attenuation = mat->albedo;
    return true;
}

void fill_hit_record(hit_record* rec, const float t, const ray* r, const sphere* s) {
    rec->t = t;
    rec->point = ray_at(r, rec->t);
    rec->normal = HMM_DivideVec3f(HMM_SubtractVec3(rec->point, s->center), s->radius);
    rec->material = s->material;
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