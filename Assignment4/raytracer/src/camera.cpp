#include "camera.h"

/* OrthographicCamera */

Ray OrthographicCamera::generateRay(Vec2f point) {
    Vec3f origin = center + (point.x() - 0.5) * size * horizontal + (point.y() - 0.5) * size * screenUp;
    return Ray(origin, direction);
}

float OrthographicCamera::getTMin() const {
    return -INFINITY;
}

void OrthographicCamera::glInit(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-size / 2.0, size / 2.0, -size * (float) h / (float) w / 2.0, size * (float) h / (float) w / 2.0, 0.5,
                40.0);
    else
        glOrtho(-size * (float) w / (float) h / 2.0, size * (float) w / (float) h / 2.0, -size / 2.0, size / 2.0, 0.5,
                40.0);
}

void OrthographicCamera::glPlaceCamera(void) {
    gluLookAt(center.x(), center.y(), center.z(),
              center.x() + direction.x(), center.y() + direction.y(), center.z() + direction.z(),
              up.x(), up.y(), up.z());
}

void OrthographicCamera::dollyCamera(float dist) {
    center += direction * dist;
}

void OrthographicCamera::truckCamera(float dx, float dy) {
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();

    Vec3f screenUp;
    Vec3f::Cross3(screenUp, horizontal, direction);

    center += horizontal * dx + screenUp * dy;
}

void OrthographicCamera::rotateCamera(float rx, float ry) {
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();

    // Don't let the model flip upside-down (There is a singularity
    // at the poles when 'up' and 'direction' are aligned)
    float tiltAngle = acos(up.Dot3(direction));
    if (tiltAngle - ry > 3.13)
        ry = tiltAngle - 3.13;
    else if (tiltAngle - ry < 0.01)
        ry = tiltAngle - 0.01;

    Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
    rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

    rotMat.Transform(center);
    rotMat.TransformDirection(direction);
}

/* PerspectiveCamera */

Ray PerspectiveCamera::generateRay(Vec2f point) {
    float dis = 0.5 / tan(angle / 2);
    Vec3f rayDir = dis * direction + (point.x() - 0.5) * horizontal + (point.y() - 0.5) * screenUp;
    rayDir.Normalize();
    return Ray(center, rayDir);
}

float PerspectiveCamera::getTMin() const {
    return 0;
}

void PerspectiveCamera::glInit(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angle * 180.0 / 3.14159, (float) w / float(h), 0.5, 40.0);
}

void PerspectiveCamera::glPlaceCamera(void) {
    gluLookAt(center.x(), center.y(), center.z(),
              center.x() + direction.x(), center.y() + direction.y(), center.z() + direction.z(),
              up.x(), up.y(), up.z());
}

void PerspectiveCamera::dollyCamera(float dist) {
    center += direction * dist;
}

void PerspectiveCamera::truckCamera(float dx, float dy) {
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();

    Vec3f screenUp;
    Vec3f::Cross3(screenUp, horizontal, direction);

    center += horizontal * dx + screenUp * dy;
}

void PerspectiveCamera::rotateCamera(float rx, float ry) {
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();

    // Don't let the model flip upside-down (There is a singularity
    // at the poles when 'up' and 'direction' are aligned)
    float tiltAngle = acos(up.Dot3(direction));
    if (tiltAngle - ry > 3.13)
        ry = tiltAngle - 3.13;
    else if (tiltAngle - ry < 0.01)
        ry = tiltAngle - 0.01;

    Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
    rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

    rotMat.Transform(center);
    rotMat.TransformDirection(direction);
    direction.Normalize();
}