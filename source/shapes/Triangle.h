//
// Created by hao on 3/6/19.
//

#ifndef PATH_TRACING_TRIANGLE_H
#define PATH_TRACING_TRIANGLE_H

#include "../core/core.h"

class Triangle : public Shape {
public:
    Triangle() = default;

    Triangle(const vec3 &_a, const vec3 &_b, const vec3 &_c,
             const vec3 &_n_a, const vec3 &_n_b, const vec3 &_n_c,
             const vec2 &_uv_a, const vec2 &_uv_b, const vec2 &_uv_c, shared_ptr<Material> m) :
            a(_a), b(_b), c(_c),
            n_a(_n_a), n_b(_n_b), n_c(_n_c),
            uv_a(_uv_a), uv_b(_uv_b), uv_c(_uv_c), mat_ptr(m) {

        vec3 cross_vector = glm::cross(b - a, c - a);
        surface_area = 0.5 * glm::length(cross_vector);
        box = AABB(vec3(std::min(std::min(a.x, b.x), c.x), std::min(std::min(a.y, b.y), c.y),
                        std::min(std::min(a.z, b.z), c.z)),
                   vec3(std::max(std::max(a.x, b.x), c.x), std::max(std::max(a.y, b.y), c.y),
                        std::max(std::max(a.z, b.z), c.z)));
    }

    bool Intersect(const Ray &ray, float t_min, float t_max, Interaction &isect) const {
        float A = obj_det3x3(a.x - b.x, a.x - c.x, ray.d.x,
                             a.y - b.y, a.y - c.y, ray.d.y,
                             a.z - b.z, a.z - c.z, ray.d.z);
        float beta = obj_det3x3(a.x - ray.o.x, a.x - c.x, ray.d.x,
                                a.y - ray.o.y, a.y - c.y, ray.d.y,
                                a.z - ray.o.z, a.z - c.z, ray.d.z) / A;
        float gamma = obj_det3x3(a.x - b.x, a.x - ray.o.x, ray.d.x,
                                 a.y - b.y, a.y - ray.o.y, ray.d.y,
                                 a.z - b.z, a.z - ray.o.z, ray.d.z) / A;
        float t = obj_det3x3(a.x - b.x, a.x - c.x, a.x - ray.o.x,
                             a.y - b.y, a.y - c.y, a.y - ray.o.y,
                             a.z - b.z, a.z - c.z, a.z - ray.o.z) / A;
        // Record hit point
        if (t < t_min || t > t_max || isnan(t) || beta + gamma > 1 || beta < 0 || gamma < 0) {
            return false;
        }
        float alpha = 1 - beta - gamma;
        isect.t = t;
        isect.p = ray(isect.t);
        get_triangle_uv(alpha, beta, gamma,
                        uv_a, uv_b, uv_c,
                        isect.u, isect.v);
        isect.n = normalize(alpha * n_a + beta * n_b + gamma * n_c);
        isect.mat_ptr = mat_ptr;
        if (mat_ptr->alpha_channel(isect) < 1e-4) {
            return false;
        }
        return true;
    }

    bool bounding_box(__attribute__((unused)) float t0, __attribute__((unused)) float t1, AABB &_box) const {
        _box = box;
        return true;
    }

    virtual float pdf_value(const vec3 &o, const vec3 &v) const {
        Interaction rec;
        if (this->Intersect(Ray(o, v, 0), 0.001, FLT_MAX, rec)) {
            float distance_squared = rec.t * rec.t * dot(v, v);
            float cosine = fabs(dot(v, rec.n) / length(v));
            if (cosine > 0) {
                return distance_squared / (cosine * surface_area);
            }
        }
        return 0;
    }

    virtual vec3 random(const vec3 &o) const {
        vec3 random_weight = random_on_triangle();
        vec3 onTriangle = random_weight.x * a + random_weight.y * b + random_weight.z * c;
        return onTriangle - o;
    }

    vec3 a, b, c;
    vec3 n_a, n_b, n_c;
    vec2 uv_a, uv_b, uv_c;
    shared_ptr<Material> mat_ptr;
    AABB box;
};

#endif //PATH_TRACING_TRIANGLE_H
