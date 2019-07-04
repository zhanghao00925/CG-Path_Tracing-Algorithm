//
// Created by hao on 2/28/19.
//

#include "MovingSphere.h"

vec3 MovingSphere::center(float time) const{
    return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
}


// replace "center" with "center(r.time())"
bool MovingSphere::Intersect(const Ray &ray, float tmin, float tmax, Interaction &isect) const {
    vec3 oc = ray.origin() - center(ray.time());
    float a = dot(ray.direction(), ray.direction());
    float b = dot(oc, ray.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < tmax && temp > tmin) {
            isect.t = temp;
            isect.p = ray(isect.t);
            get_sphere_uv((isect.p-center(ray.time()))/radius, isect.u, isect.v);
            isect.n = (isect.p - center(ray.time())) / radius;
            isect.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if (temp < tmax && temp > tmin) {
            isect.t = temp;
            isect.p = ray(isect.t);
            get_sphere_uv((isect.p-center(ray.time()))/radius, isect.u, isect.v);
            isect.n = (isect.p - center(ray.time())) / radius;
            isect.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}
