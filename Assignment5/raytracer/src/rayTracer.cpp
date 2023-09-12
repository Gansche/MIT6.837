
#include "rayTracer.h"
#include "object3d.h"

//TODO:May be bugs

Vec3f RayTracer::mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const {
    Vec3f dir = incoming - 2 * incoming.Dot3(normal) * normal;
    dir.Normalize();
    return dir;
}

bool RayTracer::transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
                                     float index_i, float index_t, Vec3f &transmitted) const {
    Vec3f N = normal;
    Vec3f I = incoming * (-1);
    N.Normalize();
    I.Normalize();
    float ratio = index_i / index_t;
    float judge = 1 - ratio * ratio * (1 - N.Dot3(I) * N.Dot3(I));
    if (judge >= 0) {
        transmitted = (ratio * N.Dot3(I) - sqrt(judge)) * N - ratio * I;
        transmitted.Normalize();
        return false;
    } else {
        transmitted = Vec3f(0.0, 0.0, 0.0);
        transmitted.Normalize();
        return true;
    };
}

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const {
    Group *group = scene->getGroup();
    Vec3f color(0.0, 0.0, 0.0);

    if (bounces > max_bounces || weight < cutoff_weight)return Vec3f(0.0, 0.0, 0.0);
    if (!group->intersect(ray, hit, tmin))return scene->getBackgroundColor();
    if (bounces == 0) RayTree::SetMainSegment(ray, 0, hit.getT());

    Material *material = hit.getMaterial();

    /*Phong shade*/
    color = color + (scene->getAmbientLight()) * (material->getDiffuseColor());

    Light *light;
    Vec3f dir, col;
    float distanceToLight;
    Vec3f point = ray.pointAtParameter(hit.getT());
    for (int i = 0; i < scene->getNumLights(); i++) {
        bool inter = false;
        light = scene->getLight(i);
        light->getIllumination(point, dir, col, distanceToLight);
        if (shadows) {
            Ray rayToLight(point, dir);
            Hit hitOfLight(distanceToLight, nullptr, Vec3f(0.0, 0.0, 0.0));
            inter = group->intersectShadowRay(rayToLight, hitOfLight, epsilon);
            RayTree::AddShadowSegment(rayToLight, 0, hitOfLight.getT());
        }
        if (!inter) {
            Vec3f phongColor = hit.getMaterial()->Shade(ray, hit, dir, col, shade_back);
            color += phongColor;
        }
    }

    /*Reflection*/
    Vec3f reflectiveColor = material->getReflectiveColor();
    if (reflectiveColor.Length() > 0) {
        Vec3f mirrorDir = mirrorDirection(hit.getNormal(), ray.getDirection());
        Ray reflectRay(point, mirrorDir);
        Hit reflectHit(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));
        Vec3f reflectColor = traceRay(reflectRay, epsilon, bounces + 1,
                                      weight * reflectiveColor.Length(), indexOfRefraction, reflectHit);
        color += reflectColor * reflectiveColor;
        RayTree::AddReflectedSegment(reflectRay, 0, reflectHit.getT());
    }

    /*Refraction*/
    Vec3f transparentColor = material->getTransparentColor();
    if (transparentColor.Length() > 0) {
        Vec3f normal = hit.getNormal();
        float index_i, index_t;
        if (ray.getDirection().Dot3(normal) > 0) {
            normal.Negate();
            index_i = hit.getMaterial()->getIndexOfRefraction();
            index_t = 1;
        } else {
            index_i = 1;
            index_t = hit.getMaterial()->getIndexOfRefraction();
        }
        Vec3f transmitted;
        bool internalReflect = transmittedDirection(normal, ray.getDirection(), index_i, index_t, transmitted);
        if (!internalReflect) {
            Ray refractRay(point, transmitted);
            Hit refractHit(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));
            Vec3f refractColor = traceRay(refractRay, epsilon, bounces + 1,
                                          weight * transparentColor.Length(), index_t, refractHit);
            color += refractColor * transparentColor;
            RayTree::AddTransmittedSegment(refractRay, 0, refractHit.getT());
        }
    }
    return color;
}