#pragma once
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "matrix.h"
#include "vectors.h"
#include "image.h"

class IFS {
public:
    IFS() : n(0), matrix(nullptr), prob(nullptr) {}

    ~IFS() {
        delete[] matrix;
        delete[] prob;
    }

    void input(const char *file);

    void render(Image &image, int num_points, int num_iters);

private:
    int n;
    Matrix *matrix;
    float *prob;
};


