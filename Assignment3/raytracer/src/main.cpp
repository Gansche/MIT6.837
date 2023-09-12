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
#include "light.h"
#include "glCanvas.h"

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

void argParser(int argc, char **argv);

void render();

int main(int argc, char **argv) {
    argParser(argc, argv);
    SceneParser *scene = new SceneParser(input_file);

    if (gui) {
        GLCanvas canvas;
        glutInit(&argc, argv);
        canvas.initialize(scene, render);
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

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Ray ray = camera->generateRay(Vec2f(float(i) / float(width), float(j) / float(height)));
            Hit hit(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));

            if (group->intersect(ray, hit, camera->getTMin())) {
                Vec3f normal = hit.getNormal();
                Vec3f direction = ray.getDirection();

                // Set the pixel of normalsImage
                normalsImage.SetPixel(i, j, Vec3f(fabs(normal.x()), fabs(normal.y()), fabs(normal.z())));

                // Set the pixel of depthImage
                float t = hit.getT();
                if (t > depth_max)
                    t = depth_max;
                if (t < depth_min)
                    t = depth_min;
                t = (depth_max - t) / (depth_max - depth_min);
                depthImage.SetPixel(i, j, Vec3f(t, t, t));

                // shade back if necessary
                if (normal.Dot3(direction) > 0 && shade_back) {
                    normal = normal * (-1);
                }

                // Set the pixel of outputImage
                Vec3f color(0.0, 0.0, 0.0);
                Vec3f ambientColor = hit.getMaterial()->getDiffuseColor() * scene.getAmbientLight();
                color = color + ambientColor;
                for (int k = 0; k < scene.getNumLights(); k++) {
                    Light *light = scene.getLight(k);
                    Vec3f dir;
                    Vec3f col;
                    float dis;
                    light->getIllumination(hit.getIntersectionPoint(), dir, col, dis);
                    color += hit.getMaterial()->Shade(ray, hit, dir, col);
                }
                outputImage.SetPixel(i, j, color);
            }
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