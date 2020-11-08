#pragma once

#include <stdio.h>
#include "math.h"

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