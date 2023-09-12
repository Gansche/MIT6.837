#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "ray.h"
#include "hit.h"
#include "material.h"
#include "LAlib/matrix.h"
#include "boundingbox.h"
#include "marchinginfo.h"
#include <vector>

class Grid;

class Object3D {
public:
    Object3D() {};

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

    virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin) = 0;

    virtual void paint() const = 0;

    virtual void insertIntoGrid(Grid *g, Matrix *m) {};

    virtual BoundingBox *getBoundingBox() = 0;

    virtual ~Object3D() {};

protected:
    Material *material;
    BoundingBox *boundingBox;
    bool isTriangle = false;
};

class Group : public Object3D {
public:
    Group(int n) : num_objects(n) {
        material = nullptr;
        boundingBox = new BoundingBox(Vec3f(INFINITY, INFINITY, INFINITY),
                                      Vec3f(-INFINITY, -INFINITY, -INFINITY));
        objects = new Object3D *[num_objects];
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override;//TODO:intersect 需要考虑是不是grid？？

    bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {
        for (int i = 0; i < num_objects; i++) {
            if (objects[i]->intersectShadowRay(r, h, tmin)) return true;
        }
        return false;
    }

    void paint() const override;

    void addObject(int index, Object3D *obj) {
        objects[index] = obj;
        BoundingBox *objBoundingBox = obj->getBoundingBox();
        if (objBoundingBox) {
            boundingBox->Extend(objBoundingBox);
        }
    }

    void insertIntoGrid(Grid *g, Matrix *m) override;

    BoundingBox *getBoundingBox() override {
        return boundingBox;
    }

    ~Group() override {
        for (int i = 0; i < num_objects; i++) {
            delete objects[i];
        }
        delete[] objects;
    }

private:
    int num_objects;
    Object3D **objects;
};

class Sphere : public Object3D {
public:
    Sphere(Vec3f _centre, float _radius, Material *_material) {
        center = _centre;
        radius = _radius;
        material = _material;
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;

    bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {
        return Sphere::intersect(r, h, tmin);
    }

    void paint() const override;

    void insertIntoGrid(Grid *g, Matrix *m) override;

    BoundingBox *getBoundingBox() override {
        boundingBox = new BoundingBox(center - Vec3f(radius, radius, radius),
                                      center + Vec3f(radius, radius, radius));
        return boundingBox;
    }

    ~Sphere() override {}

private:
    Vec3f center;
    float radius;
};

class Plane : public Object3D {
public:
    Plane(Vec3f &_normal, float _d, Material *_material) {
        normal = _normal;
        d = _d;
        material = _material;
        d = d / normal.Length();
        normal.Normalize();
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;

    bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {
        return Plane::intersect(r, h, tmin);
    }

    void paint() const override;

    void insertIntoGrid(Grid *g, Matrix *m) override;

    BoundingBox *getBoundingBox() override { return nullptr; }

    ~Plane() override {};

private:
    float d;
    Vec3f normal;
};

class Triangle : public Object3D {
public:
    Triangle(Vec3f &_a, Vec3f &_b, Vec3f &_c, Material *_material) : a(_a), b(_b), c(_c) {
        material = _material;
        Vec3f::Cross3(normal, b - a, c - a);
        normal.Normalize();
        isTriangle = true;
    };

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;

    bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {
        return Triangle::intersect(r, h, tmin);
    }

    void paint() const override;

    void insertIntoGrid(Grid *g, Matrix *m) override;

    BoundingBox *getBoundingBox() override {
        boundingBox = new BoundingBox(
                Vec3f(min(min(a.x(), b.x()), c.x()), min(min(a.y(), b.y()), c.y()), min(min(a.z(), b.z()), c.z())),
                Vec3f(max(max(a.x(), b.x()), c.x()), max(max(a.y(), b.y()), c.y()), max(max(a.z(), b.z()), c.z())));
        return boundingBox;
    }

    Vec3f getA() { return a; }

    Vec3f getB() { return b; }

    Vec3f getC() { return c; }

    ~Triangle() override {};

private:
    Vec3f a;
    Vec3f b;
    Vec3f c;
    Vec3f normal;
};

class Transform : public Object3D {
public:
    Transform(Matrix &_matrix, Object3D *_object) : matrix(_matrix), object(_object) {
        material = nullptr;
    };

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;

    bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {
        return Transform::intersect(r, h, tmin);
    }

    void paint() const override;

    void insertIntoGrid(Grid *g, Matrix *m) override;

    BoundingBox *getBoundingBox() override;

    ~Transform() override {}

private:
    Matrix matrix;
    Object3D *object;
};

class Grid : public Object3D {
public:
    Grid(BoundingBox *bb, int _nx, int _ny, int _nz) {
        material = nullptr;
        boundingBox = bb;
        nx = _nx;
        ny = _ny;
        nz = _nz;
        opaque.resize(nx * ny * nz);
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override;

    bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {}

    void paint() const override;

    Vec3f getGrid() { return Vec3f(nx, ny, nz); }

    void insertIntoThis(int index, Object3D *obj) {
        opaque[index].push_back(obj);
    }

    void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const;

    BoundingBox *getBoundingBox() override { return boundingBox; }

    ~Grid() override {}

private:
    int nx;
    int ny;
    int nz;
    vector<vector<Object3D *>> opaque;
};

#endif