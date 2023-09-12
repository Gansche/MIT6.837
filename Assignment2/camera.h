#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "ray.h"

class Camera
{
public:
    Camera() {}
    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;
    virtual ~Camera() {}
};

class OrthographicCamera : public Camera
{
public:
    OrthographicCamera(Vec3f _center, Vec3f _direction, Vec3f _up, float _size)
        : center(_center), direction(_direction), up(_up), size(_size)
    {
        direction.Normalize();
        Vec3f::Cross3(horizontal, direction, up);
        Vec3f::Cross3(up, horizontal, direction);
        up.Normalize();
        horizontal.Normalize();
    }
    Ray generateRay(Vec2f point) override;
    float getTMin() const override;
    ~OrthographicCamera() override{};

private:
    Vec3f center;
    Vec3f direction;
    Vec3f horizontal;
    Vec3f up;
    float size;
};

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera(Vec3f _center, Vec3f &_direction, Vec3f &_up, float _angle)
        : center(_center), direction(_direction), up(_up), angle(_angle)
    {
        direction.Normalize();
        Vec3f::Cross3(horizontal, direction, up);
        Vec3f::Cross3(up, horizontal, direction);
        up.Normalize();
        horizontal.Normalize();
    }
    Ray generateRay(Vec2f point) override;
    float getTMin() const
    {
        return 0;
    };
    ~PerspectiveCamera() override{};

private:
    Vec3f center;
    Vec3f direction;
    Vec3f horizontal;
    Vec3f up;
    float angle;
};

#endif