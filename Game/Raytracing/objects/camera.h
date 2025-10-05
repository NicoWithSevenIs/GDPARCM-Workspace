#pragma once
#include "../rtweekend.h"
#include <fstream>
#include <sstream>
#include "material.h"
#include "../../Threading/Task.h"
#include <vector>
#include <array>

class partition_data {
    public:
        int x,y;
        color** partition;

};

class camera {
public:
    /* Public Camera Parameters Here */
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width = 512;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth = 10;   // Maximum number of ray bounces into scene
    double vfov = 90;  // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0);   // Point camera is looking from
    point3 lookat = point3(0, 0, -1);  // Point camera is looking at
    vec3   vup = vec3(0, 1, 0);     // Camera-relative "up" direction

    int vertical_partitions = 1;
    int horizontal_partitions = 1;

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void render_whole(const hittable& world) {
        std::stringstream s;
        s << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {

                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                write_color(s, pixel_samples_scale * pixel_color);

            }
        }

        std::ofstream pic("balls.ppm");
        pic << s.str();
        pic.close();
    }

    void render_partitioned(const hittable& world) {

        vertical_partitions = std::max(vertical_partitions, 1);
        horizontal_partitions = std::max(horizontal_partitions, 1);

        int h_step = image_width / horizontal_partitions;
        int v_step = image_height / vertical_partitions;

        int h_offset = image_width % horizontal_partitions;
        int v_offset = image_height % vertical_partitions;


        std::vector<std::thread*> thread_list;


        color** c = new color*[image_height];
        for (int i = 0; i < image_height; i++) {
            c[i] = new color[image_width];
        }

        for (int j = 0; j < vertical_partitions; j++) {
            for (int i = 0; i < horizontal_partitions; i++) {
                //pass by value to prevent i value modification across threads


                auto t = Task::Spawn([=, &world, &c]() {
                    std::cout << "Task Started\n";
                    for (int h = v_step * j; h < v_step * (j + 1); h++) {
                        for (int w = h_step * i; w < h_step * (i + 1); w++) {
                            color pixel_color(0, 0, 0);
                            for (int sample = 0; sample < samples_per_pixel; sample++) {
                                ray r = get_ray(w, h);
                                pixel_color += ray_color(r, max_depth, world);
                            }

                            pixel_color = pixel_color * pixel_samples_scale;

                            auto r = pixel_color.x();
                            auto g = pixel_color.y();
                            auto b = pixel_color.z();

                            // Apply a linear to gamma transform for gamma 2
                            r = linear_to_gamma(r);
                            g = linear_to_gamma(g);
                            b = linear_to_gamma(b);

                            // Translate the [0,1] component values to the byte range [0,255].
                            int rbyte = int(255.999 * r);
                            int gbyte = int(255.999 * g);
                            int bbyte = int(255.999 * b);

                            //std::cout << rbyte << " " << gbyte << " " << bbyte << std::endl;
                            c[w][h] = color(rbyte, gbyte, bbyte);  
                        }
                    }
                    std::cout << "Task Done\n";
                    }, false);

               thread_list.push_back(t);
            }
        }


       for (auto t : thread_list)
           t->join();

        std::stringstream s;
        std::ofstream pic("balls.ppm");

        s << "P3\n" << image_width << ' ' << image_height << "\n255\n";

       
        for (int h =0; h< image_height; h++) {
            for (int w = 0; w < image_width; w++) {
                color cl = c[w][h];
                s << cl.x() << " " << cl.y() << " " << cl.z() << "\n";
            }
        }
        
         
        pic << s.str();
        pic.close();


        for(int i =0; i < image_height; i++)
            delete[] c[i];
        delete[] c;
    }
    void render(const hittable& world) {
      
        initialize();

        //render_whole(world);
        render_partitioned(world);
  
    }

private:
    /* Private Camera Variables Here */
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    vec3   u, v, w;              // Camera frame basis vectors
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions.

        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height);


        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }
    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0, 0, 0);

        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }


};
