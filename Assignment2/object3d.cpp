#include "object3d.h"
bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
    bool flag = false;
    for (int i = 0; i < num_objects; i++)
    {
        if (objects[i]->intersect(r, h, tmin))
            flag = true;
    }
    return flag;
}

void Group::addObject(int index, Object3D *obj)
{
    objects[index] = obj;
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f ro = r.getOrigin() - centre;
    Vec3f rd = r.getDirection();
    float rd_len = rd.Length();
    float tp = -ro.Dot3(rd) / (rd_len);
    if (tp < 0)
        return false;
    float d2 = ro.Dot3(ro) - tp * tp;
    if (d2 > radius * radius)
        return false;
    float tt = sqrt(radius * radius - d2);
    float t1 = tp - tt;
    float t2 = tp + tt;
    float t;
    if (ro.Length() - radius > 0)
        t = t1;
    else
        t = t2;
    t = t / rd_len;
    if (t > tmin && t < h.getT())
    {
        Vec3f normal = ro + t * rd;
        normal.Normalize();
        h.set(t, material, normal, r);
        return true;
    }
    return false;
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f ro = r.getOrigin();
    Vec3f rd = r.getDirection();
    float denom = normal.Dot3(rd);
    if (denom == 0)
        return false;
    float t = (d - normal.Dot3(ro)) / denom;
    if (t > tmin && t < h.getT())
    {
        h.set(t, material, normal, r);
        return true;
    }
    return false;
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f Ro = r.getOrigin();
    Vec3f Rd = r.getDirection();
    float A =
        det3x3(
            a.x() - b.x(), a.x() - c.x(), Rd.x(),
            a.y() - b.y(), a.y() - c.y(), Rd.y(),
            a.z() - b.z(), a.z() - c.z(), Rd.z());
    float beta =
        det3x3(
            a.x() - Ro.x(), a.x() - c.x(), Rd.x(),
            a.y() - Ro.y(), a.y() - c.y(), Rd.y(),
            a.z() - Ro.z(), a.z() - c.z(), Rd.z()) /
        A;
    float gamma =
        det3x3(a.x() - b.x(), a.x() - Ro.x(), Rd.x(),
               a.y() - b.y(), a.y() - Ro.y(), Rd.y(),
               a.z() - b.z(), a.z() - Ro.z(), Rd.z()) /
        A;
    if (beta + gamma < 1 && beta > 0 && gamma > 0)
    {
        float t =
            det3x3(a.x() - b.x(), a.x() - c.x(), a.x() - Ro.x(),
                   a.y() - b.y(), a.y() - c.y(), a.y() - Ro.y(),
                   a.z() - b.z(), a.z() - c.z(), a.z() - Ro.z()) /
            A;
        if (t > tmin && t < h.getT())
        {
            h.set(t, material, normal, r);
            return true;
        }
    }
    return false;
}

bool Transform::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f origin = r.getOrigin();
    Vec3f direction = r.getDirection();

    Matrix invMatrix = matrix;
    if (invMatrix.Inverse())
    {
        invMatrix.Transform(origin);
        invMatrix.TransformDirection(direction);
        Ray invRay(origin, direction);
        if (object->intersect(invRay, h, tmin))
        {
            Matrix invMatrixT = invMatrix;
            invMatrixT.Transpose();
            Vec3f normal = h.getNormal();
            invMatrixT.TransformDirection(normal);
            normal.Normalize();
            h.set(h.getT(), h.getMaterial(), normal, invRay);
            return true;
        }
    }
    return false;
}