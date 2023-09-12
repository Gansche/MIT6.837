#include "camera.h"

Ray OrthographicCamera::generateRay(Vec2f point)
{
    Vec3f origin = center + (point.x() - 0.5) * size * horizontal + (point.y() - 0.5) * size * up;
    return Ray(origin, direction);
}

float OrthographicCamera::getTMin() const
{
    return -INFINITY;
}

Ray PerspectiveCamera::generateRay(Vec2f point)
{
    float dis = 0.5 / tan(angle / 2);
    Vec3f rayDir = dis * direction + (point.x() - 0.5) * horizontal + (point.y() - 0.5) * up;
    rayDir.Normalize();
    return Ray(center, rayDir);
}