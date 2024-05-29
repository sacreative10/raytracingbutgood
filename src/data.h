//
// Created by sohaibalam on 29/05/24.
//

#ifndef RAYTRACINGBUTGOOD_DATA_H
#define RAYTRACINGBUTGOOD_DATA_H

#include "common.h"
#include "geometry.h"

std::vector<Vec3> diamondVert = {
    {0, 0, 10},      {4.5, 4.5, 0},  {4.5, -4.5, 0},
    {-4.5, -4.5, 0}, {-4.5, 4.5, 0}, {0, 0, -10},
};

std::vector<int> diamondIndices = {
    1, 2, 3, 1, 3, 4, 1, 4, 5, 1, 5, 2, 6, 5,
    4, 6, 4, 3, 6, 3, 2, 6, 2, 1, 6, 1, 5,
};

#endif // RAYTRACINGBUTGOOD_DATA_H
