//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_NOISETEXTURE_H
#define PATH_TRACING_NOISETEXTURE_H

#include "../core/core.h"

class NoiseTexture : public Texture {
public:
    NoiseTexture() = default;

    NoiseTexture(float sc) : scale(sc) {}

    virtual vec3 Evaluate(__attribute__((unused)) float u, __attribute__((unused)) float v, const vec3 &p) const override {
//        return vec3(1, 1, 1) * 0.5f * (1 + noise.turb(scale * p));
//        return vec3(1, 1, 1) * noise.turb(scale * p);
        return vec3(1, 1, 1) * 0.5f * (1 + sin(scale * p.z + 10 * noise.turb(scale * p)));
    }

    perlin noise;
    float scale;
};


#endif //PATH_TRACING_NOISETEXTURE_H
