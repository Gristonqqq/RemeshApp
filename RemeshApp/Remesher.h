#pragma once

#include "CoreMesh.h"

class Remesher
{
public:
    static bool remeshMesh(CoreMesh& core,
        double target_edge_length = 0.01,
        int iterations = 3);
};
