//
// Created by hao on 3/1/19.
//

#include "Rectangle.h"

bool xy_rect::Intersect(const Ray &r, float t0, float t1, Interaction &rec) const {
    float t = (k - r.origin().z) / r.direction().z;
    if (t < t0 || t > t1)
        return false;
    float x = r.origin().x + t * r.direction().x;
    float y = r.origin().y + t * r.direction().y;
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r(t);
    rec.n = vec3(0, 0, 1);
    return true;
}

bool yz_rect::Intersect(const Ray &r, float t0, float t1, Interaction &rec) const {
    float t = (k - r.origin().x) / r.direction().x;
    if (t < t0 || t > t1)
        return false;
    float y = r.origin().y + t * r.direction().y;
    float z = r.origin().z + t * r.direction().z;
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;
    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r(t);
    rec.n = vec3(1, 0, 0);
    return true;
}
