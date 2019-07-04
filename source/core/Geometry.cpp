//
// Created by hao on 2/28/19.
//

#include "Geometry.h"

AABB surrounding_box(AABB box0, AABB box1) {
    vec3 small( fmin(box0.min().x, box1.min().x),
                fmin(box0.min().y, box1.min().y),
                fmin(box0.min().z, box1.min().z));
    vec3 big  ( fmax(box0.max().x, box1.max().x),
                fmax(box0.max().y, box1.max().y),
                fmax(box0.max().z, box1.max().z));
    return {small,big};
}