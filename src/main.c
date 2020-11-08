#include <stdlib.h>
#include <stdbool.h>
#include "math.h"
#include "sphere.h"
#include "ray.h"
#include "camera.h"
#include "color.h"

#define MAX_SPHERES 2000

struct {
    camera cam;
    sphere spheres[MAX_SPHERES];
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

    if (hit_spheres(r, 0.001f, INFINITY, &hit_r)) {
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



void add_sphere(const point3 center, const float radius, const material mat) {
    if (state.spheres_length >= MAX_SPHERES) {
        return;
    }

    state.spheres[state.spheres_length] = (sphere) {
        .center = center,
        .radius = radius,
        .material = mat
    };

    ++state.spheres_length;
}

void generate_random_scene() {

    const material ground_material = mat_lambertian(HMM_Vec3(0.5f, 0.5f, 0.5f));
    add_sphere(HMM_Vec3(0.f,-1000.f,0.f), 1000.f, ground_material);

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            const float choose_mat = random_float();
            const point3 center = HMM_Vec3(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());
            const hmm_v3 distance = HMM_SubtractVec3(center, HMM_Vec3(4.f, 0.2f, 0.f));


            if (HMM_LengthVec3(distance) > 0.9f) {

                if (choose_mat < 0.8f) {
                    // diffuse
                    const color albedo = HMM_MultiplyVec3(random_v3(), random_v3());
                    add_sphere(center, 0.2f, mat_lambertian(albedo));
                } 
                else if (choose_mat < 0.95f) {
                    // metal
                    const color albedo = random_v3_interval(0.5f, 1.f);
                    const float fuzz = random_float_interval(0.f, 0.5f);
                    add_sphere(center, 0.2f, mat_metal(albedo, fuzz));
                } 
                else {
                    // glass
                    add_sphere(center, 0.2f, mat_dielectric(1.5f));
                }
            }
        }
    }

    add_sphere(HMM_Vec3(0.f, 1.f, 0.f), 1.0f, mat_dielectric(1.5f));
    add_sphere(HMM_Vec3(-4.f, 1.f, 0.f), 1.0f, mat_lambertian(HMM_Vec3(.4f, .2f, .1f)));
    add_sphere(HMM_Vec3(4.f, 1.f, 0.f), 1.0f, mat_metal(HMM_Vec3(.7f, .6f, .5f), 0.f));
}

int main() {

    // Image
    const float aspect_ratio = 3.f / 2.f;
    const int image_width = 1200;
    const int image_height = (int)(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    const hmm_v3 position = HMM_Vec3(13.f, 2.f, 3.f);
    const hmm_v3 lookat = HMM_Vec3(0.f, 0.f, 0.f);
    const hmm_v3 vup = HMM_Vec3(0.f, 1.f, 0.f);

    const float dist_to_focus = 10.f;
    const float aperture = 0.1f;

    state.cam = create_camera(&position, &lookat, &vup, 20.f, aspect_ratio, aperture, dist_to_focus);

    generate_random_scene();

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