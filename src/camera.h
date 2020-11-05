#pragma once

#include "math.h"
#include "ray.h"

typedef struct camera {
    point3 origin;
    point3 lower_left_corner;
    hmm_v3 horizontal;
    hmm_v3 vertical;
    hmm_v3 u, v, w;
    float lens_radius;
} camera;

camera create_camera(const hmm_v3* pos, const hmm_v3* lookat, const hmm_v3* vup, const float vfov, const float aspect_ratio,
    float aperture, float focus_dist) 
{
    const float theta = HMM_ToRadians(vfov);
    const float h =  HMM_TanF(theta / 2.f);
    const float viewport_height = 2.f * h;
    const float viewport_width = aspect_ratio * viewport_height;

    const hmm_v3 w = HMM_NormalizeVec3(HMM_SubtractVec3(*pos, *lookat));
    const hmm_v3 u = HMM_NormalizeVec3(HMM_Cross(*vup, w));
    const hmm_v3 v = HMM_Cross(w, u);

    const hmm_v3 horizontal = HMM_MultiplyVec3f(u, viewport_width * focus_dist);
    const hmm_v3 vertical = HMM_MultiplyVec3f(v, viewport_height * focus_dist);
    hmm_v3 lower_left_corner = HMM_SubtractVec3(*pos, HMM_DivideVec3f(horizontal, 2.f));
    lower_left_corner = HMM_SubtractVec3(lower_left_corner, HMM_DivideVec3f(vertical, 2.f));
    lower_left_corner = HMM_SubtractVec3(lower_left_corner, HMM_MultiplyVec3f(w, focus_dist));

    return (camera) {
        .origin = *pos,
        .lower_left_corner = lower_left_corner,
        .horizontal = horizontal,
        .vertical = vertical,
        .u = u,
        .v = v,
        .w = w,
        .lens_radius = aperture / 2.f
    };
}

ray get_ray(const camera* cam, const float u, const float v) {
    const hmm_v3 rd = HMM_MultiplyVec3f(random_in_unit_disk(), cam->lens_radius);
    hmm_v3 offset = HMM_MultiplyVec3f(cam->u, rd.X);
    offset = HMM_AddVec3(offset, HMM_MultiplyVec3f(cam->v, rd.Y));

    hmm_v3 direction = HMM_AddVec3(cam->lower_left_corner, HMM_MultiplyVec3f(cam->horizontal, u));
    direction = HMM_AddVec3(direction, HMM_MultiplyVec3f(cam->vertical, v));
    direction = HMM_SubtractVec3(direction, cam->origin);

    return (ray) {
        .origin = HMM_AddVec3(cam->origin, offset),
        .direction = HMM_SubtractVec3(direction, offset)
    };
}