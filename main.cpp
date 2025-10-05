
//#include "Game/Window/Window.h"
#include "opencv2/core.hpp"
#include <opencv2/imgcodecs.hpp>


#include "Game/Raytracing/RTImage/RTImage.h"
#include "Game/Raytracing/objects/camera.h"

#include "Game/Raytracing/rtweekend.h"
#include "Game/Raytracing/objects/hittable.h"
#include "Game/Raytracing/objects/hittableList.h"
#include "Game/Raytracing/objects/sphere.h"
#include "Game/Raytracing/objects/material.h"

#include <chrono>

#include "Game/Threading/Task.h"



int main() {
    


    //World

    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));


    /*
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.2 * random_double(), 0.2, b + 0.2 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }
    */
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    camera cam;

    cam.aspect_ratio = 1;
    cam.image_width = 1080;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.vertical_partitions = 2;
    cam.horizontal_partitions = 2;


    auto start = std::chrono::steady_clock::now();
    cam.render(world);
    auto end = std::chrono::steady_clock::now();


    std::chrono::duration<float> elapsed = end - start;
    std::cout << elapsed.count() << std::endl;

    auto convert_to_png = [](std::string filename) {
        auto s = cv::String(filename);
        auto mat = cv::imread(s, cv::IMREAD_COLOR);
        cv::imwrite("balls2.png", mat);
    };

    convert_to_png("balls.ppm");

    system("start balls2.png");

    return 0;
}



 
