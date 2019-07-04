//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_HOMOGENEOUSMEDIUM_H
#define PATH_TRACING_HOMOGENEOUSMEDIUM_H

#include "../core/core.h"
#include "../materials/Isotropic.h"

class HomogeneousMedium : public Shape {
public:
public:
    HomogeneousMedium(shared_ptr<Shape> b, float d, shared_ptr<Texture> a) : boundary(b), density(d) {
        phase_function = make_shared<Isotropic>(a);
    }

    virtual bool Intersect(const Ray &r, float t_min, float t_max, Interaction &rec) const;

    virtual bool bounding_box(float t0, float t1, AABB &box) const {
        return boundary->bounding_box(t0, t1, box);
    }

    shared_ptr<Shape> boundary;
    float density;
    shared_ptr<Material> phase_function;
};


#endif //PATH_TRACING_HOMOGENEOUSMEDIUM_H
