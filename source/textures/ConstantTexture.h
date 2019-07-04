//
// Created by hao on 2/28/19.
//

#ifndef PATH_TRACING_CONSTANTTEXTURE_H
#define PATH_TRACING_CONSTANTTEXTURE_H

#include "../core/core.h"

class ConstantTexture : public Texture {
public:
    ConstantTexture() = default;

    ConstantTexture(vec3 c) : color(c) {}

    virtual vec3 Evaluate(__attribute__((unused))  float u, __attribute__((unused))  float v, __attribute__((unused))  const vec3 &p) const override {
        return color;
    }

    vec3 color;

};


#endif //PATH_TRACING_CONSTANTTEXTURE_H
