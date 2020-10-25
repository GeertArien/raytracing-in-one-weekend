#pragma once

#include "math.h"
#include "ray.h"

typedef struct camera {
    point3 origin;
    point3 lower_left_corner;
    float viewport_width;
    float viewport_height;
} camera;

camera create_camera(const float aspect_ratio) {
    const float viewport_height = 2.f;
    const float viewport_width = aspect_ratio * viewport_height;
    const float focal_length = 1.f;

    return (camera) {
        .origin = HMM_Vec3(0.f, 0.f, 0.f),
        .lower_left_corner = HMM_Vec3(-viewport_width / 2.f, -viewport_height / 2.f, -focal_length),
        .viewport_width = viewport_width,
        .viewport_height = viewport_height
    };
}

ray get_ray(camera* cam, float u, float v) {
    const hmm_v3 offset = HMM_Vec3(u * cam->viewport_width, v * cam->viewport_height, 0.f);
    const hmm_v3 direction = HMM_SubtractVec3(HMM_AddVec3(cam->lower_left_corner, offset), cam->origin);

    return (ray) {
        .origin = cam->origin,
        .direction = direction
    };
}