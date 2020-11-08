#pragma once

#include "color.h"

typedef struct material {
    color albedo;
    bool reflect;
    bool dielectric;
    float fuzz;
    float ir;
} material;

material mat_lambertian(const color albedo) {
    return (material) {
        .albedo = albedo,
    };
}

material mat_metal(const color albedo, const float fuzz) {
    return (material) {
        .albedo = albedo,
        .reflect = true,
        .fuzz = fuzz
    };
}

material mat_dielectric(const float ir) {
    return (material) {
        .dielectric = true,
        .ir = ir
    };
}
