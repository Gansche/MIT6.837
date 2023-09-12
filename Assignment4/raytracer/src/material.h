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

    virtual Vec3f getSpecularColor() const { return Vec3f(0.0, 0.0, 0.0); }

    virtual Vec3f getReflectiveColor() const { return Vec3f(0.0, 0.0, 0.0); }

    virtual Vec3f getTransparentColor() const { return Vec3f(0.0, 0.0, 0.0); }

    virtual float getIndexOfRefraction() const { return 0.0; }

    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor,
                        const bool shade_back) const = 0;

    virtual void glSetMaterial(void) const = 0;

    virtual ~Material() {}

protected:
    Vec3f diffuseColor;
};

/*TODO:May be bugs:
 * 三个新增的成员放在派生类里面
 * 以及调用函数没有在基类中添加纯虚函数
 */

class PhongMaterial : public Material {
public:
    PhongMaterial(const Vec3f &_diffuseColor,
                  const Vec3f &_specularColor,
                  float _exponent,
                  const Vec3f &_reflectiveColor,
                  const Vec3f &_transparentColor,
                  float _indexOfRefraction) :
            Material(_diffuseColor),
            specularColor(_specularColor),
            exponent(_exponent),
            reflectiveColor(_reflectiveColor),
            transparentColor(_transparentColor),
            indexOfRefraction(_indexOfRefraction) {}

    Vec3f getSpecularColor() const override { return specularColor; }

    Vec3f getReflectiveColor() const override { return reflectiveColor; }

    Vec3f getTransparentColor() const override { return transparentColor; }

    float getIndexOfRefraction() const override { return indexOfRefraction; }

    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor,
                const bool shade_back) const override;

    void glSetMaterial(void) const override;

    ~PhongMaterial() override {}

private:
    Vec3f specularColor;
    float exponent;

    Vec3f reflectiveColor;
    Vec3f transparentColor;
    float indexOfRefraction;
};

#endif