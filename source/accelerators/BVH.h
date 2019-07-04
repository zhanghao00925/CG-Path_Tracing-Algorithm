//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_BVH_H
#define PATH_TRACING_BVH_H

#include "../core/core.h"

class BVHNode : public Shape {
public:
    BVHNode() = default;

    BVHNode(vector<shared_ptr<Shape>>::iterator l, int n, float time0, float time1);

    virtual bool Intersect(const Ray &ray, float t_min, float t_max, Interaction &isect) const;

    virtual bool bounding_box(float t0, float t1, AABB &box) const;

    shared_ptr<Shape> left;
    shared_ptr<Shape> right;
    AABB box;
};


bool box_x_compare(const shared_ptr<Shape> &a, const shared_ptr<Shape> &b);

bool box_y_compare(const shared_ptr<Shape> &a, const shared_ptr<Shape> &b);

bool box_z_compare(const shared_ptr<Shape> &a, const shared_ptr<Shape> &b);


#endif //PATH_TRACING_BVH_H
