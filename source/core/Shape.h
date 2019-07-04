//
// Created by hao on 2/27/19.
//

#ifndef PATH_TRACING_SHAPE_H
#define PATH_TRACING_SHAPE_H

#include "Ray.h"
#include "Interaction.h"
#include "Geometry.h"

inline float obj_det2x2(const float &a, const float &b,
                        const float &c, const float &d) {
    return a * d - b * c;
}

inline float obj_det3x3(const float &a1, const float &a2, const float &a3,
                        const float &b1, const float &b2, const float &b3,
                        const float &c1, const float &c2, const float &c3) {
    return a1 * obj_det2x2(b2, b3, c2, c3)
           - b1 * obj_det2x2(a2, a3, c2, c3)
           + c1 * obj_det2x2(a2, a3, b2, b3);
}

void get_sphere_uv(const vec3 &p, float &u, float &v);

void get_triangle_uv(const float &alpha, const float &beta, const float &gamma,
                     const vec2 &uv_a, const vec2 &uv_b, const vec2 &uv_c,
                     float &u, float &v);

class Shape {
public:
    virtual bool Intersect(const Ray &Ray, float t_min, float t_max,
                           Interaction &isect) const = 0;

    virtual bool bounding_box(float t0, float t1, AABB &box) const = 0;

    virtual float pdf_value(__attribute__((unused)) const vec3 &o,__attribute__((unused)) const vec3 &v) const { return 0.0; }

    virtual vec3 random(__attribute__((unused)) const vec3 &o) const { return vec3(1, 0, 0); }

    virtual float area() const { return surface_area; }

    float surface_area;
};


class flip_normals : public Shape {
public:
    flip_normals(shared_ptr<Shape> p) : ptr(p) {}

    virtual bool Intersect(const Ray &r, float t_min, float t_max, Interaction &rec) const {
        if (ptr->Intersect(r, t_min, t_max, rec)) {
            rec.n = -rec.n;
            return true;
        } else
            return false;
    }

    virtual bool bounding_box(float t0, float t1, AABB &box) const {
        return ptr->bounding_box(t0, t1, box);
    }

    shared_ptr<Shape> ptr;
};

class translate : public Shape {
public:
    translate(shared_ptr<Shape> p, const vec3 &displacement) : ptr(p), offset(displacement) {}

    virtual bool Intersect(const Ray &r, float t_min, float t_max, Interaction &rec) const;

    virtual bool bounding_box(float t0, float t1, AABB &box) const;

    shared_ptr<Shape> ptr;
    vec3 offset;
};

class rotate_y : public Shape {
public:
    rotate_y(shared_ptr<Shape> p, float angle);

    virtual bool Intersect(const Ray &r, float t_min, float t_max, Interaction &rec) const;

    virtual bool bounding_box(__attribute__((unused)) float t0,__attribute__((unused)) float t1, AABB &box) const {
        box = bbox;
        return hasbox;
    }

    shared_ptr<Shape> ptr;
    float sin_theta;
    float cos_theta;
    bool hasbox;
    AABB bbox;
};


#endif //PATH_TRACING_SHAPE_H
