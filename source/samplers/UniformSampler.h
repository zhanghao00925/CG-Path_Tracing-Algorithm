//
// Created by hao on 3/17/19.
//

#ifndef PATH_TRACING_UNIFORMSAMPLER_H
#define PATH_TRACING_UNIFORMSAMPLER_H

#include "../core/core.h"

class UniformSampler : public Sampler {
public:
    UniformSampler() = default;
    virtual float Get1D() {
        return drand48();
    };
    virtual vec2 Get2D() {
        return vec2(drand48(), drand48());
    };
};


#endif //PATH_TRACING_UNIFORMSAMPLER_H
