#include "object3d.h"
#include <GL/freeglut.h>
#include <vector>

#define epsilon 1e-4

/*
 * GROUP
 */

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

void Group::insertIntoGrid(Grid *g, Matrix *m) {
    for (int i = 0; i < num_objects; i++) {
        objects[i]->insertIntoGrid(g, m);
    }
}

/*
 * SPHERE
 */

bool Sphere::intersect(const Ray &r, Hit &h, float tmin) {
    Vec3f relative_origin = r.getOrigin() - center;
    float a = r.getDirection().Length() * r.getDirection().Length();
    float b = 2 * relative_origin.Dot3(r.getDirection());
    float c = relative_origin.Length() * relative_origin.Length() - radius * radius;
    float delta = b * b - 4 * a * c;
    if (delta < 0) return false;
    delta = sqrt(delta);
    float t1 = (-b + delta) / (2 * a), t2 = (-b - delta) / (2 * a);

    if (t2 > tmin && t2 < h.getT()) {
        Vec3f normal = relative_origin + t2 * r.getDirection();
        normal.Normalize();
        h.set(t2, material, normal, r);
        return true;
    } else if (t1 > tmin && t1 < h.getT()) {
        Vec3f normal = relative_origin + t1 * r.getDirection();
        normal.Normalize();
        h.set(t1, material, normal, r);
        return true;
    } else return false;
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

void Sphere::insertIntoGrid(Grid *g, Matrix *m) {
    //TODO:debug
    //FIXME:m
    BoundingBox *bb = g->getBoundingBox();
    int nx = g->getGrid().x();
    int ny = g->getGrid().y();
    int nz = g->getGrid().z();
    Vec3f gridLong = bb->getMax() - bb->getMin();
    float dx = gridLong.x() / float(nx);
    float dy = gridLong.y() / float(ny);
    float dz = gridLong.z() / float(nz);
    float diag = sqrt(dx * dx + dy * dy + dz * dz);
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            for (int k = 0; k < nz; k++) {
                Vec3f centerOfVoxel = Vec3f((i + 0.5) * dx, (j + 0.5) * dy, (k + 0.5) * dz) + bb->getMin();
                if ((centerOfVoxel - center).Length() <= radius + 0.5 * diag) {
                    g->insertIntoThis(i * ny * nz + j * nz + k, this);
                }
            }
        }
    }
    return;
}

/*
 * PLANE
 */

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

//TODO:不知道咋写，只能遍历计算了
void Plane::insertIntoGrid(Grid *g, Matrix *m) {
    BoundingBox *bb = g->getBoundingBox();
    int nx = g->getGrid().x();
    int ny = g->getGrid().y();
    int nz = g->getGrid().z();
    Vec3f gridLong = bb->getMax() - bb->getMin();
    float dx = gridLong.x() / float(nx);
    float dy = gridLong.y() / float(ny);
    float dz = gridLong.z() / float(nz);
    float diag = sqrt(dx * dx + dy * dy + dz * dz);
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            for (int k = 0; k < nz; k++) {
                Vec3f centerOfVoxel = Vec3f((i + 0.5) * dx, (j + 0.5) * dy, (k + 0.5) * dz) + bb->getMin();
                float dis = fabs(normal.Dot3(centerOfVoxel) - d) / normal.Length();
                if (dis <= 0.5 * diag) {
                    g->insertIntoThis(i * ny * nz + j * nz + k, this);
                }
            }
        }
    }
    return;
}

/*
 * TRIANGLE
 */

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
    if (beta + gamma <= 1 + epsilon && beta > 0 && gamma > 0) {
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

void Triangle::insertIntoGrid(Grid *g, Matrix *m) {
    Vec3f tri_min = boundingBox->getMin();
    Vec3f tri_max = boundingBox->getMax();

    Vec3f grid_min = g->getBoundingBox()->getMin();
    Vec3f grid_max = g->getBoundingBox()->getMax();

    int nx = g->getGrid().x();
    int ny = g->getGrid().y();
    int nz = g->getGrid().z();

    float cell_x = (grid_max - grid_min).x() / nx;
    float cell_y = (grid_max - grid_min).y() / ny;
    float cell_z = (grid_max - grid_min).z() / nz;

    int start_i = fabs(tri_min.x() - grid_min.x()) * (1 / cell_x);
    int start_j = fabs(tri_min.y() - grid_min.y()) * (1 / cell_y);
    int start_k = fabs(tri_min.z() - grid_min.z()) * (1 / cell_z);
    int end_i = fabs(tri_max.x() - grid_min.x()) * (1 / cell_x);
    int end_j = fabs(tri_max.y() - grid_min.y()) * (1 / cell_y);
    int end_k = fabs(tri_max.z() - grid_min.z()) * (1 / cell_z);

    if (start_i == end_i)end_i++;
    if (start_j == end_j)end_j++;
    if (start_k == end_k)end_k++;
    if (start_i > end_i) swap(start_i, end_i);
    if (start_j > end_j) swap(start_j, end_j);
    if (start_k > end_k) swap(start_k, end_k);

    assert(start_i >= 0 && start_i <= nx &&
           start_j >= 0 && start_j <= ny &&
           start_k >= 0 && start_k <= nz);

    for (int i = start_i; i < end_i; i++) {
        for (int j = start_j; j < end_j; j++) {
            for (int k = start_k; k < end_k; k++) {
                g->insertIntoThis(i * ny * nz + j * nz + k, this);
            }
        }
    }
}

/*
 * TRANSFORM
 */

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

BoundingBox *Transform::getBoundingBox() {
    Matrix m = matrix;
    if (isTriangle) {
        Triangle *t = (Triangle *) object;
        Vec3f a = t->getA();
        Vec3f b = t->getB();
        Vec3f c = t->getC();
        m.Transform(a);
        m.Transform(b);
        m.Transform(c);
        boundingBox = new BoundingBox(Vec3f(min(a.x(), b.x()), min(a.y(), b.y()), min(a.z(), b.z())),
                                      Vec3f(max(a.x(), b.x()), max(a.y(), b.y()), max(a.z(), b.z())));
        boundingBox->Extend(c);
    } else {
        BoundingBox *bb = object->getBoundingBox();
        Vec3f _v1 = bb->getMax();
        Vec3f _v2 = bb->getMin();
        float _x1 = _v1.x(), _y1 = _v1.y(), _z1 = _v1.z();
        float _x2 = _v2.x(), _y2 = _v2.y(), _z2 = _v2.z();
        Vec3f _v3(_x2, _y1, _z1);
        Vec3f _v4(_x2, _y2, _z1);
        Vec3f _v5(_x1, _y2, _z1);
        Vec3f _v6(_x1, _y2, _z2);
        Vec3f _v7(_x1, _y1, _z2);
        Vec3f _v8(_x2, _y1, _z2);

        m.Transform(_v1);
        m.Transform(_v2);
        m.Transform(_v3);
        m.Transform(_v4);
        m.Transform(_v5);
        m.Transform(_v6);
        m.Transform(_v7);
        m.Transform(_v8);

        _x1 = _v1.x(), _y1 = _v1.y(), _z1 = _v1.z();
        _x2 = _v2.x(), _y2 = _v2.y(), _z2 = _v2.z();
        float _x3 = _v3.x(), _y3 = _v3.y(), _z3 = _v3.z();
        float _x4 = _v4.x(), _y4 = _v4.y(), _z4 = _v4.z();
        float _x5 = _v5.x(), _y5 = _v5.y(), _z5 = _v5.z();
        float _x6 = _v6.x(), _y6 = _v6.y(), _z6 = _v6.z();
        float _x7 = _v7.x(), _y7 = _v7.y(), _z7 = _v7.z();
        float _x8 = _v8.x(), _y8 = _v8.y(), _z8 = _v8.z();

        float _xmin = _x1 < _x2 ? _x1 : _x2;
        _xmin = _xmin < _x3 ? _xmin : _x3;
        _xmin = _xmin < _x4 ? _xmin : _x4;
        _xmin = _xmin < _x5 ? _xmin : _x5;
        _xmin = _xmin < _x6 ? _xmin : _x6;
        _xmin = _xmin < _x7 ? _xmin : _x7;
        _xmin = _xmin < _x8 ? _xmin : _x8;

        float _ymin = _y1 < _y2 ? _y1 : _y2;
        _ymin = _ymin < _y3 ? _ymin : _y3;
        _ymin = _ymin < _y4 ? _ymin : _y4;
        _ymin = _ymin < _y5 ? _ymin : _y5;
        _ymin = _ymin < _y6 ? _ymin : _y6;
        _ymin = _ymin < _y7 ? _ymin : _y7;
        _ymin = _ymin < _y8 ? _ymin : _y8;

        float _zmin = _z1 < _z2 ? _z1 : _z2;
        _zmin = _zmin < _z3 ? _zmin : _z3;
        _zmin = _zmin < _z4 ? _zmin : _z4;
        _zmin = _zmin < _z5 ? _zmin : _z5;
        _zmin = _zmin < _z6 ? _zmin : _z6;
        _zmin = _zmin < _z7 ? _zmin : _z7;
        _zmin = _zmin < _z8 ? _zmin : _z8;

        float _xmax = _x1 > _x2 ? _x1 : _x2;
        _xmax = _xmax > _x3 ? _xmax : _x3;
        _xmax = _xmax > _x4 ? _xmax : _x4;
        _xmax = _xmax > _x5 ? _xmax : _x5;
        _xmax = _xmax > _x6 ? _xmax : _x6;
        _xmax = _xmax > _x7 ? _xmax : _x7;
        _xmax = _xmax > _x8 ? _xmax : _x8;

        float _ymax = _y1 > _y2 ? _y1 : _y2;
        _ymax = _ymax > _y3 ? _ymax : _y3;
        _ymax = _ymax > _y4 ? _ymax : _y4;
        _ymax = _ymax > _y5 ? _ymax : _y5;
        _ymax = _ymax > _y6 ? _ymax : _y6;
        _ymax = _ymax > _y7 ? _ymax : _y7;
        _ymax = _ymax > _y8 ? _ymax : _y8;

        float _zmax = _z1 > _z2 ? _z1 : _z2;
        _zmax = _zmax > _z3 ? _zmax : _z3;
        _zmax = _zmax > _z4 ? _zmax : _z4;
        _zmax = _zmax > _z5 ? _zmax : _z5;
        _zmax = _zmax > _z6 ? _zmax : _z6;
        _zmax = _zmax > _z7 ? _zmax : _z7;
        _zmax = _zmax > _z8 ? _zmax : _z8;

        boundingBox = new BoundingBox(Vec3f(_xmax, _ymax, _zmax), Vec3f(_xmin, _ymin, _zmin));
    }
    return boundingBox;
}

void Transform::insertIntoGrid(Grid *g, Matrix *m) {
    Vec3f m_min = boundingBox->getMin();
    Vec3f m_max = boundingBox->getMax();
    Vec3f v = g->getGrid();
    BoundingBox *bb = g->getBoundingBox();
    Vec3f min = bb->getMin();
    Vec3f max = Vec3f(bb->getMax().x(), bb->getMax().y(), bb->getMax().z());
    int x = v.x();
    int y = v.y();
    int z = v.z();
    Vec3f size = max - min;
    float grid_x = size.x() / x;
    float grid_y = size.y() / y;
    float grid_z = size.z() / z;

    int _start_i = (fabs(m_min.x() - min.x())) * (1 / grid_x);
    int _start_j = (fabs(m_min.y() - min.y())) * (1 / grid_y);
    int _start_k = (fabs(m_min.z() - min.z())) * (1 / grid_z);
    int _end_i = (fabs(m_max.x() - min.x())) * (1 / grid_x);
    int _end_j = (fabs(m_max.y() - min.y())) * (1 / grid_y);
    int _end_k = (fabs(m_max.z() - min.z())) * (1 / grid_z);

    if (_start_i == _end_i) _start_i--;
    if (_start_j == _end_j) _start_j--;
    if (_start_k == _end_k) _start_k--;
    if (_start_i > _end_i) swap(_start_i, _end_i);
    if (_start_j > _end_j) swap(_start_j, _end_j);
    if (_start_k > _end_k) swap(_start_k, _end_k);

    //assert(_end_i <= x && _end_j <= y && _end_k <= z);
    if (_end_i > x)_end_i--;
    if (_end_j > y)_end_j--;
    if (_end_k > z)_end_k--;

    for (int _i = _start_i; _i < _end_i; _i++) {
        for (int _j = _start_j; _j < _end_j; _j++) {
            for (int _k = _start_k; _k < _end_k; _k++) {
                g->insertIntoThis((_i * y + _j) * z + _k, this);
            }
        }
    }
}

/*
 * GRID
 */

PhongMaterial *getColor(int size) {
    PhongMaterial *col;
    switch (size) {
        case 1:
            col = new PhongMaterial(Vec3f(1, 1, 1));
            break;
        case 2:
            col = new PhongMaterial(Vec3f(1, 0, 1));
            break;
        case 3:
            col = new PhongMaterial(Vec3f(0, 1, 1));
            break;
        case 4:
            col = new PhongMaterial(Vec3f(1, 1, 0));
            break;
        case 5:
            col = new PhongMaterial(Vec3f(0.3, 0, 0.7));
            break;
        case 6:
            col = new PhongMaterial(Vec3f(0.7, 0, 0.3));
            break;
        case 7:
            col = new PhongMaterial(Vec3f(0, 0.3, 0.7));
            break;
        case 8:
            col = new PhongMaterial(Vec3f(0, 0.7, 0.3));
            break;
        case 9:
            col = new PhongMaterial(Vec3f(0, 0.3, 0.7));
            break;
        case 10:
            col = new PhongMaterial(Vec3f(0, 0.7, 0.3));
            break;
        case 11:
            col = new PhongMaterial(Vec3f(0, 1, 0));
            break;
        case 12:
            col = new PhongMaterial(Vec3f(0, 0, 1));
            break;
        default:
            col = new PhongMaterial(Vec3f(1, 0, 0));
            break;
    }
    return col;
}

void Grid::paint() const {
    //TODO:May be some bugs, but complexity decrease more than 2/3!!
    PhongMaterial *col = nullptr;
    PhongMaterial *col_x = nullptr;
    PhongMaterial *col_y = nullptr;
    PhongMaterial *col_z = nullptr;
    Vec3f min = boundingBox->getMin();
    Vec3f max = boundingBox->getMax();
    float dx = (max - min).x() / float(nx);
    float dy = (max - min).y() / float(ny);
    float dz = (max - min).z() / float(nz);
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            for (int k = 0; k < nz; k++) {
                Vec3f o = min + Vec3f(i * dx, j * dy, k * dz);
                Vec3f x = o + Vec3f(dx, 0.0, 0.0);
                Vec3f y = o + Vec3f(0.0, dy, 0.0);
                Vec3f z = o + Vec3f(0.0, 0.0, dz);
                Vec3f o_ = o + Vec3f(dx, dy, dz);
                Vec3f x_ = o + Vec3f(0.0, dy, dz);
                Vec3f y_ = o + Vec3f(dx, 0.0, dz);
                Vec3f z_ = o + Vec3f(dx, dy, 0.0);
                int index = i * ny * nz + j * nz + k;
                int index_x = (i + 1) * ny * nz + j * nz + k;
                int index_y = i * ny * nz + (j + 1) * nz + k;
                int index_z = i * ny * nz + j * nz + (k + 1);
                bool isOpaque = !opaque[index].empty();
                bool isOpaque_x = (i == nx - 1) ? false : !opaque[index_x].empty();
                bool isOpaque_y = (j == ny - 1) ? false : !opaque[index_y].empty();
                bool isOpaque_z = (k == nz - 1) ? false : !opaque[index_z].empty();
                col = getColor(opaque[index].size());
                col_x = getColor(opaque[index_x].size());
                col_y = getColor(opaque[index_y].size());
                col_z = getColor(opaque[index_z].size());
                col->glSetMaterial();
                if (i == 0 && isOpaque) {
                    glBegin(GL_QUADS);
                    glNormal3f(1, 0, 0);
                    glVertex3f(o.x(), o.y(), o.z());
                    glVertex3f(y.x(), y.y(), y.z());
                    glVertex3f(x_.x(), x_.y(), x_.z());
                    glVertex3f(z.x(), z.y(), z.z());
                    glEnd();
                }
                if (j == 0 && isOpaque) {
                    glBegin(GL_QUADS);
                    glNormal3f(0, -1, 0);
                    glVertex3f(o.x(), o.y(), o.z());
                    glVertex3f(x.x(), x.y(), x.z());
                    glVertex3f(y_.x(), y_.y(), y_.z());
                    glVertex3f(z.x(), z.y(), z.z());
                    glEnd();
                }
                if (k == 0 && isOpaque) {
                    glBegin(GL_QUADS);
                    glNormal3f(0, 0, 1);
                    glVertex3f(o.x(), o.y(), o.z());
                    glVertex3f(x.x(), x.y(), x.z());
                    glVertex3f(z_.x(), z_.y(), z_.z());
                    glVertex3f(y.x(), y.y(), y.z());
                    glEnd();
                }
                if (isOpaque ^ isOpaque_x) {
                    if (isOpaque)col->glSetMaterial();
                    else col_x->glSetMaterial();
                    glBegin(GL_QUADS);
                    glNormal3f(1, 0, 0);
                    glVertex3f(x.x(), x.y(), x.z());
                    glVertex3f(z_.x(), z_.y(), z_.z());
                    glVertex3f(o_.x(), o_.y(), o_.z());
                    glVertex3f(y_.x(), y_.y(), y_.z());
                    glEnd();
                }
                if (isOpaque ^ isOpaque_y) {
                    if (isOpaque)col->glSetMaterial();
                    else col_y->glSetMaterial();
                    glBegin(GL_QUADS);
                    glNormal3f(0, -1, 0);
                    glVertex3f(x_.x(), x_.y(), x_.z());
                    glVertex3f(y.x(), y.y(), y.z());
                    glVertex3f(z_.x(), z_.y(), z_.z());
                    glVertex3f(o_.x(), o_.y(), o_.z());
                    glEnd();
                }
                if (isOpaque ^ isOpaque_z) {
                    if (isOpaque)col->glSetMaterial();
                    else col_z->glSetMaterial();
                    glBegin(GL_QUADS);
                    glNormal3f(0, 0, 1);
                    glVertex3f(x_.x(), x_.y(), x_.z());
                    glVertex3f(z.x(), z.y(), z.z());
                    glVertex3f(y_.x(), y_.y(), y_.z());
                    glVertex3f(o_.x(), o_.y(), o_.z());
                    glEnd();
                }
                delete col;
                delete col_x;
                delete col_y;
                delete col_z;
            }
        }
    }
}

void Grid::initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const {
    Vec3f ro = r.getOrigin();
    Vec3f rd = r.getDirection();
    rd.Normalize();
    Vec3f min_Box = boundingBox->getMin();
    Vec3f max_Box = boundingBox->getMax();
    float cell_x = (max_Box - min_Box).x() / nx;
    float cell_y = (max_Box - min_Box).y() / ny;
    float cell_z = (max_Box - min_Box).z() / nz;

    //x_slab
    float t_x_near;
    float t_x_fal;
    if (fabs(rd.Dot3(Vec3f(1.0, 0.0, 0.0))) < epsilon) {
        t_x_near = (min_Box - ro).x() > 0 ? INFINITY : -INFINITY;
        t_x_fal = (max_Box - ro).x() > 0 ? INFINITY : -INFINITY;
    } else {
        t_x_near = (min_Box - ro).x() / rd.x();
        t_x_fal = (max_Box - ro).x() / rd.x();
        if (t_x_near > t_x_fal) {
            float temp = t_x_near;
            t_x_near = t_x_fal;
            t_x_fal = temp;
        }
    }

    //y_slab
    float t_y_near;
    float t_y_fal;
    if (fabs(rd.Dot3(Vec3f(0.0, 1.0, 0.0))) < epsilon) {
        t_y_near = (min_Box - ro).y() > 0 ? INFINITY : -INFINITY;
        t_y_fal = (max_Box - ro).y() > 0 ? INFINITY : -INFINITY;
    } else {
        t_y_near = (min_Box - ro).y() / rd.y();
        t_y_fal = (max_Box - ro).y() / rd.y();
        if (t_y_near > t_y_fal) {
            float temp = t_y_near;
            t_y_near = t_y_fal;
            t_y_fal = temp;
        }
    }

    //z_slab
    float t_z_near;
    float t_z_fal;
    if (fabs(rd.Dot3(Vec3f(0.0, 0.0, 1.0))) < epsilon) {
        t_z_near = (min_Box - ro).z() > 0 ? INFINITY : -INFINITY;
        t_z_fal = (max_Box - ro).z() > 0 ? INFINITY : -INFINITY;
    } else {
        t_z_near = (min_Box - ro).z() / rd.z();
        t_z_fal = (max_Box - ro).z() / rd.z();
        if (t_z_near > t_z_fal) {
            float temp = t_z_near;
            t_z_near = t_z_fal;
            t_z_fal = temp;
        }
    }

    float t_near = max(max(t_x_near, t_y_near), t_z_near);
    float t_far = min(min(t_x_fal, t_y_fal), t_z_fal);

    if (t_far < tmin || t_near > t_far) {
        //no intersection
        //FIXME:mi constructor √
        return;
    } else {
        //intersection
        mi.sign_x = (rd.x() > 0) ? 1 : -1;
        mi.sign_y = (rd.y() > 0) ? 1 : -1;
        mi.sign_z = (rd.z() > 0) ? 1 : -1;

        mi.dt_x = fabs(cell_x / rd.x());
        mi.dt_y = fabs(cell_y / rd.y());
        mi.dt_z = fabs(cell_z / rd.z());

        if (t_near < tmin) {
            //inside
            if (t_x_near > -INFINITY) {
                while (t_x_near <= t_near)t_x_near += mi.dt_x;
                mi.t_next_x = t_x_near;
            }
            if (t_y_near > -INFINITY) {
                while (t_y_near <= t_near)t_y_near += mi.dt_y;
                mi.t_next_y = t_y_near;
            }
            if (t_z_near > -INFINITY) {
                while (t_z_near <= t_near)t_z_near += mi.dt_z;
                mi.t_next_z = t_z_near;
            }

            t_x_near = t_x_near > tmin ? t_x_near : INFINITY;
            t_y_near = t_y_near > tmin ? t_y_near : INFINITY;
            t_z_near = t_z_near > tmin ? t_z_near : INFINITY;

            t_near = min(min(t_x_near, t_y_near), t_z_near);
            mi.tmin = t_near;

            if (t_near == t_x_near) {
                mi.normal = Vec3f(-1.0, 0.0, 0.0) * mi.sign_x;
                mi.t_next_x = t_x_near + mi.dt_x;
                mi.t_next_y = t_y_near;
                mi.t_next_z = t_z_near;
            }
            if (t_near == t_y_near) {
                mi.normal = Vec3f(0.0, -1.0, 0.0) * mi.sign_y;
                mi.t_next_x = t_x_near;
                mi.t_next_y = t_y_near + mi.dt_y;
                mi.t_next_z = t_z_near;
            }
            if (t_near == t_z_near) {
                mi.normal = Vec3f(0.0, 0.0, -1.0) * mi.sign_z;
                mi.t_next_x = t_x_near;
                mi.t_next_y = t_y_near;
                mi.t_next_z = t_z_near + mi.dt_z;
            }
        } else {
            //outside
            mi.tmin = t_near;
            if (t_near == t_x_near)mi.normal = Vec3f(-1.0, 0.0, 0.0) * mi.sign_x;
            if (t_near == t_y_near)mi.normal = Vec3f(0.0, -1.0, 0.0) * mi.sign_y;
            if (t_near == t_z_near)mi.normal = Vec3f(0.0, 0.0, -1.0) * mi.sign_z;

            if (t_x_near > -INFINITY) {
                while (t_x_near <= t_near)t_x_near += mi.dt_x;
                mi.t_next_x = t_x_near;
            }
            if (t_y_near > -INFINITY) {
                while (t_y_near <= t_near)t_y_near += mi.dt_y;
                mi.t_next_y = t_y_near;
            }
            if (t_z_near > -INFINITY) {
                while (t_z_near <= t_near)t_z_near += mi.dt_z;
                mi.t_next_z = t_z_near;
            }
        }
        Vec3f p = ro + rd * t_near - min_Box;
        mi.i = int(p.x() / cell_x);
        if (mi.sign_x < 0 && mi.i == nx)mi.i--;
        mi.j = int(p.y() / cell_y);
        if (mi.sign_y < 0 && mi.j == ny)mi.j++;
        mi.k = int(p.z() / cell_z);
        if (mi.sign_z < 0 && mi.k == nz)mi.k++;
    }
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin) {
    MarchingInfo mi;
    initializeRayMarch(mi, r, tmin);

    if (mi.tmin < h.getT()) {
        while (mi.i >= 0 && mi.j >= 0 && mi.k >= 0 &&
               mi.i < nx && mi.j < ny && mi.k < nz) {
            int i = mi.i;
            int j = mi.j;
            int k = mi.k;
            int index = i * ny * nz + j * nz + k;
            if (!opaque[index].empty()) {
                PhongMaterial *m = getColor(opaque[index].size());
                h.set(mi.tmin, m, mi.normal, r);
                return true;
            }
            mi.nextCell();
        }
    }
    return false;
}

/*
 * TODO:INFINITY?
 * TODO:t_next
 * TODO:normal,ijk
 */