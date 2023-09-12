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