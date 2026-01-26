#pragma once
#include <cstddef>

struct RemeshParams
{
    // Isotropic
    double targetEdgeLength = 0.1;
    int iterations = 3;

    // Subdivide
    int subdivLevels = 1;

    // Decimate
    std::size_t decimateTargetFaces = 5000;
    double decimateRatio = 0.0;
};
