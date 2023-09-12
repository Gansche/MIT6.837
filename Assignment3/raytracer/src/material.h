#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <GL/freeglut.h>
#include "LAlib/vectors.h"
#include "ray.h"
#include "hit.h"

class Material {
public:
    Material(const Vec3f &d_color) { diffuseColor = d_color; }

    virtual Vec3f getDiffuseColor() const { return diffuseColor; }

    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const = 0;

    virtual void glSetMaterial(void) const = 0;

    virtual ~Material() {}

protected:
    Vec3f diffuseColor;
};

class PhongMaterial : public Material {
public:
    PhongMaterial(const Vec3f &_diffuseColor, const Vec3f &_specularColor, float _exponent) :
            Material(_diffuseColor), specularColor(_specularColor), exponent(_exponent) {};

    //TODO: Is this right & useful?
    Vec3f getSpecularColor() const { return specularColor; }

    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const override;

    //TODO:override?
    void glSetMaterial(void) const override;

    ~PhongMaterial() override {}

private:
    Vec3f specularColor;
    float exponent;
};

#endif