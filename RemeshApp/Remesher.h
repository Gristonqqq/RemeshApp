#pragma once

#include "Mesh.h"

class Remesher
{
public:
    static void remeshMesh(Mesh& mesh, double target_edge_length = 0.01, int iterations = 3);
};
