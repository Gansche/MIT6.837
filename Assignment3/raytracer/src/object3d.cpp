#include "object3d.h"
#include <GL/freeglut.h>
#include <vector>

bool Group::intersect(const Ray &r, Hit &h, float tmin) {
    bool flag = false;
    for (int i = 0; i < num_objects; i++) {
        if (objects[i]->intersect(r, h, tmin))
            flag = true;
    }
    return flag;
}

void Group::paint() const {
    for (int i = 0; i < num_objects; i++) {
        objects[i]->paint();
    }
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin) {
    Vec3f ro = r.getOrigin() - center;
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
    if (t > tmin && t < h.getT()) {
        Vec3f normal = ro + t * rd;
        normal.Normalize();
        h.set(t, material, normal, r);
        return true;
    }
    return false;
}

extern int thetaStep;
extern int phiStep;
extern bool gouraud;

void Sphere::paint() const {
    material->glSetMaterial();

    float dt = 2 * M_PI / thetaStep;
    float dp = M_PI / phiStep;

    float theta = 0;
    float phi = (-1) * M_PI / 2;

    glBegin(GL_QUADS);
    for (int i = 0; i < thetaStep; i++) {
        phi = (-1) * M_PI / 2;
        for (int j = 0; j < phiStep; j++) {
            //TODO:May be a bug
            Vec3f normal[4] = {
                    Vec3f(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi)),
                    Vec3f(cos(phi + dp) * cos(theta), cos(phi + dp) * sin(theta), sin(phi + dp)),
                    Vec3f(cos(phi + dp) * cos(theta + dt), cos(phi + dp) * sin(theta + dt), sin(phi + dp)),
                    Vec3f(cos(phi) * cos(theta + dt), cos(phi) * sin(theta + dt), sin(phi))
            };
            Vec3f vertex[4] = {
                    center + radius * normal[0],
                    center + radius * normal[1],
                    center + radius * normal[2],
                    center + radius * normal[3]
            };
            if (!gouraud) {
                Vec3f n;
                Vec3f::Cross3(n, vertex[3] - vertex[1], vertex[2] - vertex[0]);
                n.Normalize();
                glNormal3f(n.x(), n.y(), n.z());
                for (int k = 0; k < 4; k++) glVertex3f(vertex[k].x(), vertex[k].y(), vertex[k].z());
            } else {
                for (int k = 0; k < 4; k++) {
                    glNormal3f(normal[k].x(), normal[k].y(), normal[k].z());
                    glVertex3f(vertex[k].x(), vertex[k].y(), vertex[k].z());
                }
            }
            phi += dp;
        }
        theta += dt;
    }
    glEnd();
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin) {
    Vec3f ro = r.getOrigin();
    Vec3f rd = r.getDirection();
    float denom = normal.Dot3(rd);
    if (denom == 0)
        return false;
    float t = (d - normal.Dot3(ro)) / denom;
    if (t > tmin && t < h.getT()) {
        h.set(t, material, normal, r);
        return true;
    }
    return false;
}

//TODO:maybe some bugs
void Plane::paint() const {
    const int INF = 10000;

    material->glSetMaterial();

    Vec3f n = normal;
    Vec3f v(1.0, 0.0, 0.0);
    n.Normalize();
    v.Normalize();
    if (fabs(n.Dot3(v) - 1) < 0.01) {
        v = Vec3f(0.0, 1.0, 0.0);
    }

    Vec3f basis_1, basis_2;
    Vec3f::Cross3(basis_1, v, n);
    Vec3f::Cross3(basis_2, n, basis_1);
    basis_1.Normalize();
    basis_2.Normalize();

    Vec3f center = d * normal;
    Vec3f vertex[4] = {
            center + basis_1 * INF + basis_2 * INF,
            center + basis_1 * INF - basis_2 * INF,
            center - basis_1 * INF - basis_2 * INF,
            center - basis_1 * INF + basis_2 * INF
    };

    glBegin(GL_QUADS);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(vertex[0].x(), vertex[0].y(), vertex[0].z());
    glVertex3f(vertex[1].x(), vertex[1].y(), vertex[1].z());
    glVertex3f(vertex[2].x(), vertex[2].y(), vertex[2].z());
    glVertex3f(vertex[3].x(), vertex[3].y(), vertex[3].z());
    glEnd();
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin) {
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
    if (beta + gamma < 1 && beta > 0 && gamma > 0) {
        float t =
                det3x3(a.x() - b.x(), a.x() - c.x(), a.x() - Ro.x(),
                       a.y() - b.y(), a.y() - c.y(), a.y() - Ro.y(),
                       a.z() - b.z(), a.z() - c.z(), a.z() - Ro.z()) /
                A;
        if (t > tmin && t < h.getT()) {
            h.set(t, material, normal, r);
            return true;
        }
    }
    return false;
}

void Triangle::paint() const {
    material->glSetMaterial();
    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(a.x(), a.y(), a.z());
    glVertex3f(b.x(), b.y(), b.z());
    glVertex3f(c.x(), c.y(), c.z());
    glEnd();
}

bool Transform::intersect(const Ray &r, Hit &h, float tmin) {
    Vec3f origin = r.getOrigin();
    Vec3f direction = r.getDirection();

    Matrix invMatrix = matrix;
    if (invMatrix.Inverse()) {
        invMatrix.Transform(origin);
        invMatrix.TransformDirection(direction);
        Ray invRay(origin, direction);
        if (object->intersect(invRay, h, tmin)) {
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

void Transform::paint() const {
    glPushMatrix();
    GLfloat *glMatrix = matrix.glGet();
    glMultMatrixf(glMatrix);
    delete[] glMatrix;
    object->paint();
    glPopMatrix();
}