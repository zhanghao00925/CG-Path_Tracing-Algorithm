//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_CHECKERBOARDTEXTURE_H
#define PATH_TRACING_CHECKERBOARDTEXTURE_H

#include "../core/core.h"

class CheckerboardTexture : public Texture {
public:
    CheckerboardTexture() = default;

    CheckerboardTexture(shared_ptr<Texture> t0, shared_ptr<Texture> t1) : even(t0), odd(t1) {}

    virtual vec3 Evaluate(float u, float v, const vec3 &p) const {
        float sines = sin(10.0f * p.x) * sin(10.0f * p.y) * sin(10.0f * p.z);
        if (sines < 0) {
            return odd->Evaluate(u, v, p);
        } else {
            return even->Evaluate(u, v, p);
        }
    }

    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;

};


#endif //PATH_TRACING_CHECKERBOARDTEXTURE_H
