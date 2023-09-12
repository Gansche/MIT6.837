#ifndef _CAMERA_H_
#define _CAMERA_H_

//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL/freeglut.h>
#include "ray.h"
#include "LAlib/vectors.h"
#include "LAlib/matrix.h"

class Camera {
public:
    Camera() {}

    virtual Ray generateRay(Vec2f point) = 0;

    virtual float getTMin() const = 0;

    virtual void glInit(int w, int h) = 0;

    virtual void glPlaceCamera(void) = 0;

    virtual void dollyCamera(float dist) = 0;

    virtual void truckCamera(float dx, float dy) = 0;

    virtual void rotateCamera(float rx, float ry) = 0;

    virtual ~Camera() {}
};

class OrthographicCamera : public Camera {
public:
    OrthographicCamera(Vec3f _center, Vec3f _direction, Vec3f _up, float _size)
            : center(_center), direction(_direction), up(_up), size(_size) {
        direction.Normalize();
        Vec3f::Cross3(horizontal, direction, up);
        Vec3f::Cross3(screenUp, horizontal, direction);
        up.Normalize();
        horizontal.Normalize();
        screenUp.Normalize();
    }

    Ray generateRay(Vec2f point) override;

    float getTMin() const override;

    void glInit(int w, int h) override;

    void glPlaceCamera(void) override;

    void dollyCamera(float dist) override;

    void truckCamera(float dx, float dy) override;

    void rotateCamera(float rx, float ry) override;

    ~OrthographicCamera() override {};

private:
    Vec3f center;
    Vec3f direction;
    Vec3f horizontal;
    Vec3f up;
    Vec3f screenUp;
    float size;
};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(Vec3f _center, Vec3f &_direction, Vec3f &_up, float _angle)
            : center(_center), direction(_direction), up(_up), angle(_angle) {
        Vec3f _screen_up(0.0, 0.0, 0.0);
        direction.Normalize();
        Vec3f::Cross3(horizontal, direction, up);
        Vec3f::Cross3(screenUp, horizontal, direction);
        up.Normalize();
        horizontal.Normalize();
        screenUp.Normalize();
    }

    Ray generateRay(Vec2f point) override;

    float getTMin() const override;

    void glInit(int w, int h) override;

    void glPlaceCamera(void) override;

    void dollyCamera(float dist) override;

    void truckCamera(float dx, float dy) override;

    void rotateCamera(float rx, float ry) override;

    ~PerspectiveCamera() override {};

private:
    Vec3f center;
    Vec3f direction;
    Vec3f horizontal;
    Vec3f up;
    Vec3f screenUp;
    float angle;
};

#endif