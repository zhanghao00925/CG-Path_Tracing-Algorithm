//
// Created by hao on 2/27/19.
//

#ifndef PATH_TRACING_CAMERA_H
#define PATH_TRACING_CAMERA_H


#include "core_std.h"
#include "../ext/ext.h"

inline vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0f * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
    } while (dot(p, p) >= 1.0);
    return p;
}

class Ray;

class Camera {
public:
    Camera(vec3 lookfrom, vec3 lookat, vec3 vup,
           float vfov, float aspect, float aperture, float focus_dist,
           float t0, float t1);

    virtual Ray GenerateRay(float s, float t);

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float time0, time1;
    float lens_radius;

};


#endif //PATH_TRACING_CAMERA_H
