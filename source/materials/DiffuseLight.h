//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_DIFFUSELIGHT_H
#define PATH_TRACING_DIFFUSELIGHT_H

#include "../core/core.h"

class DiffuseLight : public Material {
public:
    explicit DiffuseLight(shared_ptr<Texture> a) : emit(a) {}

    virtual bool
    scatter(__attribute__((unused))  const Ray &r_in, __attribute__((unused))  const Interaction &isec,
            __attribute__((unused))  ScatterRecord &srec) const {
        return false;
    }

    virtual vec3 emitted(const Ray &r_in, const Interaction &isect, float u, float v, const vec3 &p) const {
        if (dot(isect.n, r_in.direction()) < 0.0)
            return emit->Evaluate(u, v, p);
        else
            return vec3(0, 0, 0);
    }

    shared_ptr<Texture> emit;
};


#endif //PATH_TRACING_DIFFUSELIGHT_H
