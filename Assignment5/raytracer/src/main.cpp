#include <GL/freeglut.h>
#include <iostream>
#include <cstring>
#include <assert.h>
#include "scene_parser.h"
#include "Imglib/image.h"
#include "LAlib/vectors.h"
#include "ray.h"
#include "camera.h"
#include "object3d.h"
#include "glCanvas.h"
#include "rayTracer.h"

typedef bool b;
using namespace std;

/* Arguments List */
int width = 100;
int height = 100;

float depth_min = 0;
float depth_max = 1;

int thetaStep = 0;
int phiStep = 0;

char *input_file = NULL;
char *output_file = NULL;
char *depth_file = NULL;
char *normals_file = NULL;

bool shade_back = false;
bool gouraud = false;
bool gui = false;
bool shadows = false;

int max_bounces = 0;
float cutoff_weight = 0.0;

int nx = 0;
int ny = 0;
int nz = 0;

bool gridOrNot = false;
bool visualize_grid = false;

void argParser(int argc, char **argv);

void render();

void glRayTracer(float x, float y);

int main(int argc, char **argv) {
    argParser(argc, argv);
    SceneParser *scene = new SceneParser(input_file);

    Grid *grid = nullptr;
    if (nx != 0 && ny != 0 && nz != 0) {
        grid = new Grid(scene->getGroup()->getBoundingBox(), nx, ny, nz);
        scene->getGroup()->insertIntoGrid(grid, nullptr);
    }
    if (gui) {
        GLCanvas canvas;
        glutInit(&argc, argv);
        canvas.initialize(scene, render, glRayTracer, grid, visualize_grid);
    } else render();
}

void argParser(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-input")) {
            i++;
            assert(i < argc);
            input_file = argv[i];
        } else if (!strcmp(argv[i], "-size")) {
            i++;
            assert(i < argc);
            width = atoi(argv[i]);
            i++;
            assert(i < argc);
            height = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-output")) {
            i++;
            assert(i < argc);
            output_file = argv[i];
        } else if (!strcmp(argv[i], "-depth")) {
            i++;
            assert(i < argc);
            depth_min = atof(argv[i]);
            i++;
            assert(i < argc);
            depth_max = atof(argv[i]);
            i++;
            assert(i < argc);
            depth_file = argv[i];
        } else if (!strcmp(argv[i], "-normals")) {
            i++;
            assert(i < argc);
            normals_file = argv[i];
        } else if (!strcmp(argv[i], "-shade_back")) {
            shade_back = true;
        } else if (!strcmp(argv[i], "-gui")) {
            gui = true;
        } else if (!strcmp(argv[i], "-tessellation")) {
            i++;
            assert(i < argc);
            thetaStep = atoi(argv[i]);
            i++;
            assert(i < argc);
            phiStep = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-gouraud")) {
            gouraud = true;
        } else if (!strcmp(argv[i], "-bounces")) {
            i++;
            assert(i < argc);
            max_bounces = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-weight")) {
            i++;
            assert(i < argc);
            cutoff_weight = atof(argv[i]);
        } else if (!strcmp(argv[i], "-shadows")) {
            shadows = true;
        } else if (!strcmp(argv[i], "-grid")) {
            gridOrNot = true;
            i++;
            assert(i < argc);
            nx = atoi(argv[i]);
            i++;
            assert(i < argc);
            ny = atoi(argv[i]);
            i++;
            assert(i < argc);
            nz = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-visualize_grid")) {
            visualize_grid = true;
        } else {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }
}

void render() {
    SceneParser scene(input_file);
    Camera *camera = scene.getCamera();
    Object3D *group = scene.getGroup();
    Vec3f ambientLight = scene.getAmbientLight();

    Image outputImage(width, height);
    outputImage.SetAllPixels(scene.getBackgroundColor());
    Image depthImage(width, height);
    depthImage.SetAllPixels(Vec3f(0.0, 0.0, 0.0));
    Image normalsImage(width, height);
    normalsImage.SetAllPixels(Vec3f(0.0, 0.0, 0.0));

    RayTracer rayTracer(&scene, max_bounces, cutoff_weight, shadows, shade_back,
                        gridOrNot, nx, ny, nz, visualize_grid);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Ray ray = camera->generateRay(Vec2f(float(i) / float(width), float(j) / float(height)));
            Hit hit(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));

            Vec3f pixel_color = rayTracer.traceRay(ray, camera->getTMin(), 0, 1.0, 1.0, hit);
            outputImage.SetPixel(i, j, pixel_color);

            Vec3f normal = hit.getNormal();
            normalsImage.SetPixel(i, j, Vec3f(fabs(normal.x()), fabs(normal.y()), fabs(normal.z())));
            float t = hit.getT();
            if (t > depth_max)
                t = depth_max;
            if (t < depth_min)
                t = depth_min;
            t = (depth_max - t) / (depth_max - depth_min);
            depthImage.SetPixel(i, j, Vec3f(t, t, t));
        }
    }

    if (output_file != NULL)
        outputImage.SaveTGA(output_file);
    if (depth_file != NULL)
        depthImage.SaveTGA(depth_file);
    if (normals_file != NULL)
        normalsImage.SaveTGA(normals_file);
    return;
};

void glRayTracer(float x, float y) {
    SceneParser parser = SceneParser(input_file);
    Camera *c = parser.getCamera();
    RayTracer tracer(&parser, max_bounces, cutoff_weight, shadows, shade_back, gridOrNot, nx, ny, nz, visualize_grid);

    int size = width < height ? width : height;
    float step = 1.0 / size;
    Hit h;
    Ray r = c->generateRay(Vec2f(x, y));
    h.set(INFINITY, NULL, Vec3f(0, 0, 0), Ray());
    tracer.traceRay(r, c->getTMin(), 0, 1, 1, h);
}