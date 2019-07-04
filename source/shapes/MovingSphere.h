//
// Created by hao on 2/28/19.
//

#ifndef PATH_TRACING_MOVINGSPHERE_H
#define PATH_TRACING_MOVINGSPHERE_H

#include "../core/core.h"

class MovingSphere : public Shape {
public:
    MovingSphere() {}

    MovingSphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, shared_ptr<Material> m)
            : center0(cen0), center1(cen1),
              time0(t0), time1(t1), radius(r),
              mat_ptr(m) {
        surface_area = 4.0f * M_PI;
    };

    virtual bool Intersect(const Ray &ray, float tmin, float tmax, Interaction &isect) const;

    bool bounding_box(float t0, float t1, AABB &box) const {
        AABB box0(center(t0) - vec3(radius, radius, radius), center(t0) + vec3(radius, radius, radius));
        AABB box1(center(t1) - vec3(radius, radius, radius), center(t1) + vec3(radius, radius, radius));
        box = surrounding_box(box0, box1);
        return true;
    }

    vec3 center(float time) const;

    vec3 center0, center1;
    float time0, time1;
    float radius;
    shared_ptr<Material> mat_ptr;

};


#endif //PATH_TRACING_MOVINGSPHERE_H
