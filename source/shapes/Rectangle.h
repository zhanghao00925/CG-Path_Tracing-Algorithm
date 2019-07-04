//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_RECTANGLE_H
#define PATH_TRACING_RECTANGLE_H

#include "../core/core.h"

class xy_rect : public Shape {
public:
    xy_rect() {}

    xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, shared_ptr<Material> mat)
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {
        surface_area = (x1 - x0) * (y1 - y0);
    };

    virtual bool Intersect(const Ray &r, float t0, float t1, Interaction &rec) const;

    virtual bool bounding_box(__attribute__((unused))  float t0, __attribute__((unused)) float t1, AABB &box) const {
        box = AABB(vec3(x0, y0, k - 0.0001), vec3(x1, y1, k + 0.0001));
        return true;
    }

    virtual float pdf_value(const vec3 &o, const vec3 &v) const {
        Interaction rec;
        if (this->Intersect(Ray(o, v, 0), 0.001, FLT_MAX, rec)) {
            float distance_squared = rec.t * rec.t * dot(v, v);
            float light_cosine = fabs(dot(v, rec.n) / length(v));
            return distance_squared / (light_cosine * surface_area);
        } else
            return 0;
    }

    virtual vec3 random(const vec3 &o) const {
        vec3 random_point = vec3(x0 + drand48() * (x1 - x0), y0 + drand48() * (y1 - y0), k);
        return random_point - o;
    }

    float x0, x1, y0, y1, k;
    shared_ptr<Material> mp;
};

class xz_rect : public Shape {
public:
    xz_rect() {}

    xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, shared_ptr<Material> mat)
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {
        surface_area = (x1 - x0) * (z1 - z0);
    };

    virtual bool Intersect(const Ray &r, float t0, float t1, Interaction &rec) const {
        float t = (k - r.origin().y) / r.direction().y;
        if (t < t0 || t > t1)
            return false;
        float x = r.origin().x + t * r.direction().x;
        float z = r.origin().z + t * r.direction().z;
        if (x < x0 || x > x1 || z < z0 || z > z1)
            return false;
        rec.u = (x - x0) / (x1 - x0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;
        rec.mat_ptr = mp;
        rec.p = r(t);
        rec.n = vec3(0, 1, 0);;
        return true;
    }

    virtual bool bounding_box(__attribute__((unused))  float t0, __attribute__((unused)) float t1, AABB &box) const {
        box = AABB(vec3(x0, k - 0.0001, z0), vec3(x1, k + 0.0001, z1));
        return true;
    }

    virtual float pdf_value(const vec3 &o, const vec3 &v) const {
        Interaction rec;
        if (this->Intersect(Ray(o, v, 0), 0.001, FLT_MAX, rec)) {
            float distance_squared = rec.t * rec.t * dot(v, v);
            float light_cosine = fabs(dot(v, rec.n) / length(v));
            return distance_squared / (light_cosine * surface_area);
        } else
            return 0;
    }

    virtual vec3 random(const vec3 &o) const {
        vec3 random_point = vec3(x0 + drand48() * (x1 - x0), k, z0 + drand48() * (z1 - z0));
        return random_point - o;
    }

    float x0, x1, z0, z1, k;
    shared_ptr<Material> mp;
};

class yz_rect : public Shape {
public:
    yz_rect() {}

    yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, shared_ptr<Material> mat)
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {
        surface_area = (z1 - z0) * (y1 - y0);
    };

    yz_rect(vec3 center, vec2 size, shared_ptr<Material> mat) : mp(mat) {
        y0 = center.y - size.x / 2;
        y1 = center.y + size.x / 2;
        z0 = center.z - size.y / 2;
        z1 = center.z + size.y / 2;
        k = center.x;

        surface_area = (z1 - z0) * (y1 - y0);
    };

    virtual bool Intersect(const Ray &r, float t0, float t1, Interaction &rec) const;

    virtual bool bounding_box(__attribute__((unused)) float t0, __attribute__((unused)) float t1, AABB &box) const {
        box = AABB(vec3(k - 0.0001, y0, z0), vec3(k + 0.0001, y1, z1));
        return true;
    }

    virtual float pdf_value(const vec3 &o, const vec3 &v) const {
        Interaction rec;
        if (this->Intersect(Ray(o, v, 0), 0.001, FLT_MAX, rec)) {
            float distance_squared = rec.t * rec.t * dot(v, v);
            float light_cosine = fabs(dot(v, rec.n) / length(v));
            return distance_squared / (light_cosine * surface_area);
        } else
            return 0;
    }

    virtual vec3 random(const vec3 &o) const {
        vec3 random_point = vec3(k, y0 + drand48() * (y1 - y0), z0 + drand48() * (z1 - z0));
        return random_point - o;
    }

    float y0, y1, z0, z1, k;
    shared_ptr<Material> mp;
};


#endif //PATH_TRACING_RECTANGLE_H
