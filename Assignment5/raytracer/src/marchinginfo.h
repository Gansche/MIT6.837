#ifndef RAYTRACER_MARCHINGINFO_H
#define RAYTRACER_MARCHINGINFO_H

#include "LAlib/vectors.h"

class MarchingInfo {
public:
    MarchingInfo() {
        tmin = INFINITY;
        t_next_x = INFINITY;
        t_next_y = INFINITY;
        t_next_z = INFINITY;
    }

    void nextCell() {
        assert(t_next_x >= 0 && t_next_y >= 0 && t_next_z >= 0);
        assert(dt_x >= 0 && dt_y >= 0 && dt_z >= 0);

        if (t_next_x < t_next_y) {
            if (t_next_x < t_next_z) {
                i += sign_x;
                tmin = t_next_x;
                t_next_x += dt_x;
                normal = Vec3f(-1.0, 0.0, 0.0) * sign_x;
            } else {
                k += sign_z;
                tmin = t_next_z;
                t_next_z += dt_z;
                normal = Vec3f(0.0, 0.0, -1.0) * sign_z;
            }
        } else {
            if (t_next_y < t_next_z) {
                j += sign_y;
                tmin = t_next_y;
                t_next_y += dt_y;
                normal = Vec3f(0.0, -1.0, 0.0) * sign_y;
            } else {
                k += sign_z;
                tmin = t_next_z;
                t_next_z += dt_z;
                normal = Vec3f(0.0, 0.0, -1.0) * sign_z;
            }
        }
    }

    float tmin;
    float i, j, k;
    float t_next_x, t_next_y, t_next_z;
    float dt_x, dt_y, dt_z;
    float sign_x, sign_y, sign_z;
    Vec3f normal;
};

#endif //RAYTRACER_MARCHINGINFO_H
