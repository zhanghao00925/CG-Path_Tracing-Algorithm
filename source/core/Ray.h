//
// Created by hao on 2/27/19.
//

#ifndef PATH_TRACING_RAY_H
#define PATH_TRACING_RAY_H

#include <algorithm>
#include <iostream>

using namespace std;

#include "glm/glm.hpp"

using namespace glm;

class Ray {
public:
    Ray() {};

    Ray(const vec3 &_origin, const vec3 &_direction, float _time, bool _is_shadow_ray = false)
            : o(_origin), d(_direction), t(_time), bIsShadowRay(_is_shadow_ray) {
    }

    vec3 origin() const { return o; }

    vec3 direction() const { return d; }

    float time() const { return t; }

    vec3 operator()(float t) const { return o + d * t; }

    vec3 o, d;
    float t;
    bool bIsShadowRay;
};


#endif //PATH_TRACING_RAY_H
