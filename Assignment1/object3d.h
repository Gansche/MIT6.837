#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "hit.h"
#include "ray.h"
#include "material.h"

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

#endif