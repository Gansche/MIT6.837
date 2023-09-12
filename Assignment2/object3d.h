#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "hit.h"
#include "ray.h"
#include "material.h"
#include "LAlib/matrix.h"

class Object3D
{
public:
    Object3D(){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    virtual ~Object3D(){};

protected:
    Material *material;
};

class Group : public Object3D
{
public:
    Group(int n) : num_objects(n)
    {
        objects = new Object3D *[num_objects];
    }
    bool intersect(const Ray &r, Hit &h, float tmin) override;
    void addObject(int index, Object3D *obj);
    ~Group() override
    {
        for (int i = 0; i < num_objects; i++)
        {
            delete objects[i];
        }
        delete[] objects;
    }

private:
    int num_objects;
    Object3D **objects;
};

class Sphere : public Object3D
{
public:
    Sphere(Vec3f _centre, float _radius, Material *_material)
    {
        centre = _centre;
        radius = _radius;
        material = _material;
    }
    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;
    ~Sphere() override {}

private:
    Vec3f centre;
    float radius;
};

// The normal has a problem TODO
class Plane : public Object3D
{
public:
    Plane(Vec3f &_normal, float _d, Material *_material)
    {
        normal = _normal;
        d = _d;
        material = _material;
        normal.Normalize();
    }
    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;
    ~Plane() override{};

private:
    float d;
    Vec3f normal;
};

class Triangle : public Object3D
{
public:
    Triangle(Vec3f &_a, Vec3f &_b, Vec3f &_c, Material *_material) : a(_a), b(_b), c(_c)
    {
        material = _material;
        Vec3f::Cross3(normal, b - a, c - a);
        normal.Normalize();
    };
    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;
    ~Triangle() override{};

private:
    Vec3f a;
    Vec3f b;
    Vec3f c;
    Vec3f normal;
};

class Transform : public Object3D
{
public:
    Transform(Matrix &_matrix, Object3D *_object) : matrix(_matrix), object(_object){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;
    ~Transform() override {}

private:
    Matrix matrix;
    Object3D *object;
};

#endif