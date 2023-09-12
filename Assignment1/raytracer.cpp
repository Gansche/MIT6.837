#include <iostream>
#include <cstring>
#include <assert.h>
#include "scene_parser.h"
#include "Imglib/image.h"
#include "ray.h"
#include "camera.h"
#include "object3d.h"

using namespace std;

int main(int argc, char **argv)
{
    // ========================================================
    // ========================================================
    // Some sample code you might like to use for parsing
    // command line arguments

    char *input_file = NULL;
    int width = 100;
    int height = 100;
    char *output_file = NULL;
    float depth_min = 0;
    float depth_max = 1;
    char *depth_file = NULL;

    // sample command line:
    // raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-input"))
        {
            i++;
            assert(i < argc);
            input_file = argv[i];
        }
        else if (!strcmp(argv[i], "-size"))
        {
            i++;
            assert(i < argc);
            width = atoi(argv[i]);
            i++;
            assert(i < argc);
            height = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-output"))
        {
            i++;
            assert(i < argc);
            output_file = argv[i];
        }
        else if (!strcmp(argv[i], "-depth"))
        {
            i++;
            assert(i < argc);
            depth_min = atof(argv[i]);
            i++;
            assert(i < argc);
            depth_max = atof(argv[i]);
            i++;
            assert(i < argc);
            depth_file = argv[i];
        }
        else
        {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    SceneParser scene(input_file);
    Camera *camera = scene.getCamera();
    Object3D *group = scene.getGroup();

    Image outputImage(width, height);
    outputImage.SetAllPixels(scene.getBackgroundColor());
    Image depthImage(width, height);
    depthImage.SetAllPixels(Vec3f(0.0, 0.0, 0.0));

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            Ray ray = camera->generateRay(Vec2f(float(i) / float(width), float(j) / float(height)));
            Hit hit(INFINITY, nullptr);
            if (group->intersect(ray, hit, camera->getTMin()))
            {
                outputImage.SetPixel(i, j, hit.getMaterial()->getDiffuseColor());
                float t = hit.getT();
                if (t > depth_max)
                    t = depth_max;
                if (t < depth_min)
                    t = depth_min;
                t = (depth_max - t) / (depth_max - depth_min);
                depthImage.SetPixel(i, j, Vec3f(t, t, t));
            }
        }
    }

    outputImage.SaveTGA(output_file);
    depthImage.SaveTGA(depth_file);

    // ========================================================
    // ========================================================
}