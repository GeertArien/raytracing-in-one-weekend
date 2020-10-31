#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "math.h"
#include "sphere.h"
#include "ray.h"
#include "camera.h"

struct {
    camera cam;
    sphere spheres[4];
    unsigned int spheres_length;
} state;

bool hit_spheres(const ray* r, float t_min, float t_max, hit_record* rec) {
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (size_t i = 0; i < state.spheres_length; ++i) {
        if (hit_sphere(state.spheres + i, r, t_min, closest_so_far, rec)) {
            hit_anything = true;
            closest_so_far = rec->t;
        }
    }

    return hit_anything;
}

color ray_color(const ray* r, int depth) {

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0) {
        return HMM_Vec3(0.f, 0.f, 0.f);
    }

    hit_record hit_r;

    if (hit_spheres(r, 0.0001f, INFINITY, &hit_r)) {
        ray scattered;
        color attenuation;
        
        if (scatter_ray(&hit_r.material, r, &hit_r, &attenuation, &scattered)) {
            return HMM_MultiplyVec3(attenuation, ray_color(&scattered, depth - 1));
        }

        return HMM_Vec3(0.f, 0.f, 0.f);
    }

    hmm_v3 unit_direction = HMM_NormalizeVec3(r->direction);
    const float t = 0.5f * (unit_direction.Y + 1.f);
    color color0 = HMM_MultiplyVec3f(HMM_Vec3(1.f, 1.f, 1.f), (1.f - t));
    color color1 = HMM_MultiplyVec3f(HMM_Vec3(0.5f, 0.7f, 1.f), t);
    return HMM_AddVec3(color0, color1);
}

void write_color(FILE* stream, color pixel_color) {
    // gamma-correct for gamma=2.0
    const color corrected = HMM_Vec3(
        HMM_SquareRootF(pixel_color.R), 
        HMM_SquareRootF(pixel_color.G),
        HMM_SquareRootF(pixel_color.B)
    );

    // Write the translated [0,255] value of each color component.
    const int ir = (int)(256.f * HMM_Clamp(0.f, corrected.R, 0.999f));
    const int ig = (int)(256.f * HMM_Clamp(0.f, corrected.G, 0.999f));
    const int ib = (int)(256.f * HMM_Clamp(0.f, corrected.B, 0.999f));

    fprintf(stream, "%i %i %i\n", ir, ig, ib);
}

int main() {

    // Image
    const float aspect_ratio = 16.f / 9.f;
    const int image_width = 400;
    const int image_height = (int)(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    state.cam = create_camera(aspect_ratio);

    // Scene
    state.spheres[0] = (sphere) {
        .center = HMM_Vec3(0.f, -100.5f, -1.f),
        .radius = 100.f,
        .material = {
            .albedo = HMM_Vec3(0.8f, 0.8f, 0.f),
            .reflect = false
        }
    };

    state.spheres[1] = (sphere) {
        .center = HMM_Vec3(0.f, 0.f, -1.f),
        .radius = 0.5f,
        .material = {
            .albedo = HMM_Vec3(0.7f, 0.3f, 0.3f),
            .reflect = false,
            .dielectric = true,
            .ir = 1.5f
        }
    };

    state.spheres[2] = (sphere) {
        .center = HMM_Vec3(-1.f, 0.f, -1.f),
        .radius = 0.5f,
        .material = {
            .albedo = HMM_Vec3(0.8f, 0.8f, 0.8f),
            .reflect = false,
            .dielectric = true,
            .ir = 1.5f,
            .fuzz = 0.3f
        }
    };

    state.spheres[3] = (sphere) {
        .center = HMM_Vec3(1.f, 0.f, -1.f),
        .radius = 0.5f,
        .material = {
            .albedo = HMM_Vec3(0.8f, 0.6f, 0.2f),
            .reflect = true,
            .fuzz = 1.0
        }
    };

    state.spheres_length = 4;

    // Render
    printf("P3\n");
    printf("%i %i\n", image_width, image_height);
    printf("255\n");

    for (int j = image_height-1; j >= 0; --j) {
        fprintf(stderr, "\rScanlines remaining: %i", j);
        fflush(stderr);

        for (int i = 0; i < image_width; ++i) {
            color pixel_color = HMM_Vec3(0.f, 0.f, 0.f);
            
            for (int s = 0; s < samples_per_pixel; ++s) {
                const float u = ((float) i + random_float()) / ((float) image_width - 1.f);
                const float v = ((float) j + random_float()) / ((float) image_height - 1.f);
                const ray r = get_ray(&state.cam, u, v);
                pixel_color = HMM_AddVec3(pixel_color, ray_color(&r, max_depth));
            }

            // Divide the color by the number of samples.
            const float scale = 1.f / samples_per_pixel;
            pixel_color = HMM_MultiplyVec3f(pixel_color, scale);

            write_color(stdout, pixel_color);
        }
    }

    fprintf(stderr, "\nDone.\n");
}