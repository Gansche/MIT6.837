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
    float tp = -ro.Dot3(rd);
    float d2 = ro.Dot3(ro) - tp * tp;
    if (d2 > radius * radius)
        return false;
    float tt = sqrt(radius * radius - d2);
    float t1 = tp - tt;
    float t2 = tp + tt;
    float t = t1;
    if (t > tmin && t < h.getT())
    {
        h.set(t, material, r);
        return true;
    }
    return false;
}