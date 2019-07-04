//
// Created by hao on 3/1/19.
//

#include "HomogeneousMedium.h"

bool HomogeneousMedium::Intersect(const Ray &r, float t_min, float t_max, Interaction &rec) const {
    Interaction rec1, rec2;
    if (boundary->Intersect(r, -FLT_MAX, FLT_MAX, rec1)) {
        if (boundary->Intersect(r, static_cast<float>(rec1.t + 0.0001), FLT_MAX, rec2)) {
            if (rec1.t < t_min)
                rec1.t = t_min;
            if (rec2.t > t_max)
                rec2.t = t_max;
            if (rec1.t >= rec2.t)
                return false;
            if (rec1.t < 0)
                rec1.t = 0;
            float distance_inside_boundary = (rec2.t - rec1.t) * length(r.direction());
            float hit_distance = static_cast<float>(-(1 / density) * log(drand48()));
            if (hit_distance < distance_inside_boundary) {
                rec.t = rec1.t + hit_distance / length(r.direction());
                rec.p = r(rec.t);
                rec.n = vec3(1, 0, 0); // arbitrary
                rec.mat_ptr = phase_function;
                return true;
            }
        }
    }
    return false;

}
