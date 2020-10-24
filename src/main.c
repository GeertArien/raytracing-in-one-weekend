#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_NO_SSE
#include "hmm/HandmadeMath.h"

// Type aliases for vec3
typedef hmm_v3 point3;   // 3D point
typedef hmm_v3 color;    // RGB color

typedef struct ray {
    point3 origin;
    hmm_v3 direction;
} ray;

typedef struct sphere {
    point3 center;
    float radius;
    color color;
} sphere;

sphere _sphere0;

point3 ray_at(const ray* r, const float t) {
    const hmm_v3 offset = HMM_MultiplyVec3f(r->direction, t);
    return HMM_AddVec3(r->origin, offset);
};

float hit_sphere(const sphere* s, const ray* r) {
    const hmm_v3 oc = HMM_SubtractVec3(r->origin, s->center);
    const float a = HMM_LengthSquaredVec3(r->direction);
    const float half_b = HMM_DotVec3(oc, r->direction);
    const float c = HMM_LengthSquaredVec3(oc) - s->radius * s->radius;
    const float discriminant = half_b * half_b - a * c;

    if (discriminant < 0.f) {
        return -1.f;
    } else {
        return (-half_b - sqrt(discriminant)) / a;
    }

}

color ray_color(const ray* r) {
    float t = hit_sphere(&_sphere0, r);

    if (t > 0.f) {
        hmm_vec3 N = HMM_NormalizeVec3(HMM_SubtractVec3(ray_at(r, t), _sphere0.center));
        return HMM_MultiplyVec3f(HMM_Vec3(N.X + 1.f, N.Y + 1.f, N.Z + 1.f), .5f);
    }

    hmm_v3 unit_direction = HMM_NormalizeVec3(r->direction);
    t = 0.5f * (unit_direction.Y + 1.f);
    color color0 = HMM_MultiplyVec3f(HMM_Vec3(1.f, 1.f, 1.f), (1.0 - t));
    color color1 = HMM_MultiplyVec3f(HMM_Vec3(0.5f, 0.7f, 1.f), t);
    return HMM_AddVec3(color0, color1);
}

void write_color(FILE* stream, color pixel_color) {
    // Write the translated [0,255] value of each color component.
    const int ir = (int)(255.999 * pixel_color.R);
    const int ig = (int)(255.999 * pixel_color.G);
    const int ib = (int)(255.999 * pixel_color.B);

    fprintf(stream, "%i %i %i\n", ir, ig, ib);
}

int main() {

    // Image
    const float aspect_ratio = 16.f / 9.f;
    const int image_width = 400;
    const int image_height = (int)(image_width / aspect_ratio);

    // Camera
    const float viewport_height = 2.f;
    const float viewport_width = aspect_ratio * viewport_height;
    const float focal_length = 1.f;

    const point3 origin = HMM_Vec3(0.f, 0.f, 0.f);
    const point3 lower_left_corner = HMM_Vec3(-viewport_width / 2.f, -viewport_height / 2.f, -focal_length);

    // Scene
    _sphere0 = (sphere) {
        .center = HMM_Vec3(0.f, 0.f, -1.f),
        .radius = 0.5f,
        .color = HMM_Vec3(1.f, 0.f, 0.f)
    };

    // Render
    printf("P3\n");
    printf("%i %i\n", image_width, image_height);
    printf("255\n");

    for (int j = image_height-1; j >= 0; --j) {
        fprintf(stderr, "\rScanlines remaining: %i", j);
        fflush(stderr);
        for (int i = 0; i < image_width; ++i) {
            const float u = (float) i / (image_width - 1);
            const float v = (float) j / (image_height - 1);
            const hmm_v3 offset = HMM_Vec3(u * viewport_width, v * viewport_height, 0.f);
            const hmm_v3 direction = HMM_SubtractVec3(HMM_AddVec3(lower_left_corner, offset), origin);

            ray r = {
                .origin = origin,
                .direction = direction
            };

            color pixel_color = ray_color(&r);
            write_color(stdout, pixel_color);
        }
    }

    fprintf(stderr, "\nDone.\n");
}