//
// Created by hao on 2/27/19.
//

#include "Camera.h"
#include "core.h"

Camera::Camera(vec3 lookfrom, vec3 lookat, vec3 vup,
               float vfov, float aspect, float aperture, float focus_dist,
               float t0, float t1) { // vfov is top to bottom in degrees
    time0 = t0;
    time1 = t1;
    lens_radius = aperture / 2;
    float theta = vfov * M_PI / 180;
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;
    origin = lookfrom;
    w = normalize(lookfrom - lookat);
    u = normalize(cross(vup, w));
    v = cross(w, u);
    lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
    horizontal = 2 * half_width * focus_dist * u;
    vertical = 2 * half_height * focus_dist * v;
}

Ray Camera::GenerateRay(float s, float t) {
    vec3 rd = lens_radius * random_in_unit_disk();
    vec3 offset = u * rd.x + v * rd.y;
    float time = time0 + drand48() * (time1 - time0);
    return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, time);
}