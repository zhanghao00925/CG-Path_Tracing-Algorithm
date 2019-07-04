//
// Created by hao on 3/17/19.
//

#ifndef PATH_TRACING_SAMPLER_H
#define PATH_TRACING_SAMPLER_H

#include <algorithm>
#include <iostream>
using namespace std;

#include "glm/glm.hpp"
using namespace glm;

class Sampler {
public:
    Sampler() = default;
    virtual float Get1D() = 0;
    virtual vec2 Get2D() = 0;
};


#endif //PATH_TRACING_SAMPLER_H
