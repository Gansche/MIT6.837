#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H

#include "scene_parser.h"
#include "rayTree.h"
#include "light.h"
#include "object3d.h"

#define epsilon 1e-4

class RayTracer {
public:
    RayTracer(SceneParser *_scene, int _max_bounces, float _cutoff_weight, bool _shadows, bool _shade_back,
              bool _grid, int _nx, int _ny, int _nz, bool _visualize_grid) :
            scene(_scene), max_bounces(_max_bounces), cutoff_weight(_cutoff_weight), shadows(_shadows),
            shade_back(_shade_back), visualize_grid(_visualize_grid) {
        if (_grid) {
            grid = new Grid(_scene->getGroup()->getBoundingBox(), _nx, _ny, _nz);
            _scene->getGroup()->insertIntoGrid(grid, nullptr);
        } else grid = nullptr;
    }

    Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const;

    bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
                              float index_i, float index_t, Vec3f &transmitted) const;

    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const;

private:
    SceneParser *scene;
    int max_bounces;
    float cutoff_weight;
    bool shadows;
    bool shade_back;
    Grid *grid;
    bool visualize_grid;
};

#endif //RAYTRACER_RAYTRACER_H
