//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_Box_H
#define PATH_TRACING_Box_H


#include "../core/core.h"
#include "Rectangle.h"
#include "ShapeList.h"

class Box : public Shape {
public:
    Box() = default;

    Box(const vec3 &p0, const vec3 &p1, shared_ptr<Material> ptr) {
        pMin = p0;
        pMax = p1;
        vector<shared_ptr<Shape>> list(6);
        list[0] = make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p1.z, ptr);
        list[1] = make_shared<flip_normals>(make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p0.z, ptr));
        list[2] = make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p1.y, ptr);
        list[3] = make_shared<flip_normals>(make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p0.y, ptr));
        list[4] = make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p1.x, ptr);
        list[5] = make_shared<flip_normals>(make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p0.x, ptr));
        list_ptr = make_shared<ShapeList>(list);
    }

    virtual bool Intersect(const Ray &r, float t0, float t1, Interaction &rec) const {
        return list_ptr->Intersect(r, t0, t1, rec);

    }

    virtual bool bounding_box(__attribute__((unused)) float t0, __attribute__((unused)) float t1, AABB &box) const {
        box = AABB(pMin, pMax);
        return true;
    }

    vec3 pMin, pMax;
    shared_ptr<Shape> list_ptr;
};

#endif //PATH_TRACING_Box_H
