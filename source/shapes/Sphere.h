//
// Created by hao on 2/27/19.
//

#ifndef PATH_TRACING_SPHERE_H
#define PATH_TRACING_SPHERE_H

#include "../core/core.h"

//#define Sample_Half_Sphere

inline vec3 random_to_sphere(float radius, float distance_squared) {
    float r1 = drand48();
    float r2 = drand48();
    float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
    float phi = 2 * M_PI * r1;
    float x = cos(phi) * sqrt(1 - z * z);
    float y = sin(phi) * sqrt(1 - z * z);
    return vec3(x, y, z);
}

class Sphere : public Shape {
public:
    Sphere() = default;

    Sphere(vec3 _center, float r, shared_ptr<Material> m) : center(_center), radius(r), mat_ptr(m) {
        surface_area = 4.0f * M_PI * radius * radius;
    }

    bool Intersect(const Ray &ray, float t_min, float t_max, Interaction &isect) const {
        vec3 oc = ray.origin() - center;
        float a = dot(ray.direction(), ray.direction());
        float b = dot(oc, ray.direction());
        float c = dot(oc, oc) - radius * radius;
        float discriminant = b * b - a * c;
        if (discriminant > 0) {
            float temp = (-b - sqrt(b * b - a * c)) / a;
            if (temp < t_max && temp > t_min) {
                isect.t = temp;
                isect.p = ray(isect.t);
                get_sphere_uv((isect.p - center) / radius, isect.u, isect.v);
                isect.n = (isect.p - center) / radius;
                isect.mat_ptr = mat_ptr;
                return true;
            }
            temp = (-b + sqrt(b * b - a * c)) / a;
            if (temp < t_max && temp > t_min) {
                isect.t = temp;
                isect.p = ray(isect.t);
                get_sphere_uv((isect.p - center) / radius, isect.u, isect.v);
                isect.n = (isect.p - center) / radius;
                isect.mat_ptr = mat_ptr;
                return true;
            }
        }
        return false;
    }

    bool bounding_box(__attribute__((unused))  float t0, __attribute__((unused)) float t1, AABB &box) const {
        box = AABB(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
        return true;
    }

    virtual float pdf_value(const vec3 &o, const vec3 &v) const {
        Interaction rec;
#ifndef Sample_Half_Sphere
        float tmin = 1e-5;
        float pdf = 0;
        while (this->Intersect(Ray(o, v, 0), tmin, FLT_MAX, rec)) {
            float distance_squared = rec.t * rec.t * dot(v, v);
            float cosine = fabs(dot(v, rec.n) / length(v));
            pdf += distance_squared / (cosine * surface_area);
            tmin = rec.t + 1e-5;
        }
        return pdf;
#else
        if (this->Intersect(Ray(o, v, 0), 0.001, FLT_MAX, rec)) {
            float cos_theta_max = sqrt(1 - radius * radius / dot(center - o, center - o));
            float solid_angle = 2 * M_PI * (1 - cos_theta_max);
            return 1 / solid_angle;
        } else
            return 0;
#endif
    }

    virtual vec3 random(const vec3 &o) const {
#ifndef Sample_Half_Sphere
        vec3 direction = normalize(random_in_unit_sphere()) * radius + center - o;
        return direction;
#else
        vec3 direction = center - o;
        float distance_squared = dot(direction, direction);
        ONB uvw;
        uvw.build_from_w(direction);
        return uvw.local(random_to_sphere(radius, distance_squared));
#endif
    }

    vec3 center;
    float radius;
    shared_ptr<Material> mat_ptr;
};


#endif //PATH_TRACING_SPHERE_H
