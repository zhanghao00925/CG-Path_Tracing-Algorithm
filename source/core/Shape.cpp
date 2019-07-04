//
// Created by hao on 2/27/19.
//

#include "Shape.h"

void get_sphere_uv(const vec3 &p, float &u, float &v) {
    float phi = atan2(p.z, p.x);
    float theta = asin(p.y);
    u = 1 - (phi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}

void get_triangle_uv(const float &alpha, const float &beta, const float &gamma,
                     const vec2 &uv_a, const vec2 &uv_b, const vec2 &uv_c,
                     float &u, float &v) {
    vec2 uv = alpha * uv_a + beta * uv_b + gamma * uv_c;
    u = uv.x;
    v = uv.y;
}

rotate_y::rotate_y(shared_ptr<Shape> p, float angle) : ptr(p) {
    float radians = (M_PI / 180.) * angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);
    vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                float x = i * bbox.max().x + (1 - i) * bbox.min().x;
                float y = j * bbox.max().y + (1 - j) * bbox.min().y;
                float z = k * bbox.max().z + (1 - k) * bbox.min().z;
                float newx = cos_theta * x + sin_theta * z;
                float newz = -sin_theta * x + cos_theta * z;
                vec3 tester(newx, y, newz);
                for (int c = 0; c < 3; c++) {
                    if (tester[c] > max[c])
                        max[c] = tester[c];
                    if (tester[c] < min[c])
                        min[c] = tester[c];
                }
            }
        }
    }
    bbox = AABB(min, max);
}

bool rotate_y::Intersect(const Ray &r, float t_min, float t_max, Interaction &rec) const {
    vec3 origin = r.origin();
    vec3 direction = r.direction();
    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];
    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];
    Ray rotated_r(origin, direction, r.time());
    if (ptr->Intersect(rotated_r, t_min, t_max, rec)) {
        vec3 p = rec.p;
        vec3 normal = rec.n;
        p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
        p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];
        normal[0] = cos_theta * rec.n[0] + sin_theta * rec.n[2];
        normal[2] = -sin_theta * rec.n[0] + cos_theta * rec.n[2];
        rec.p = p;
        rec.n = normal;
        return true;
    } else
        return false;
}

bool translate::Intersect(const Ray &r, float t_min, float t_max, Interaction &rec) const {
    Ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (ptr->Intersect(moved_r, t_min, t_max, rec)) {
        rec.p += offset;
        return true;
    } else
        return false;
}

bool translate::bounding_box(float t0, float t1, AABB &box) const {
    if (ptr->bounding_box(t0, t1, box)) {
        box = AABB(box.min() + offset, box.max() + offset);
        return true;
    } else
        return false;
}