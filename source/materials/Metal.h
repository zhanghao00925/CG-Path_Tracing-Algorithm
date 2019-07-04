//
// Created by hao on 2/27/19.
//

#ifndef PATH_TRACING_METAL_H
#define PATH_TRACING_METAL_H

#include "../core/core.h"

class Metal : public Material {
public:
    explicit Metal(const vec3 &_albedo, float f = 0.0) : albedo(_albedo) {
        if (f < 1) {
            fuzz = f;
        } else {
            fuzz = 1.0f;
        }
    }

    virtual bool scatter(const Ray &r_in, const Interaction &isec, ScatterRecord &srec) const {
        vec3 reflected = reflect(normalize(r_in.direction()), isec.n);
        srec.specular_ray = Ray(isec.p, reflected + fuzz * random_in_unit_sphere(), r_in.t);
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = albedo;
        return dot(srec.specular_ray.direction(), isec.n) > 0;
    }

    vec3 albedo;
    float fuzz;
};


#endif //PATH_TRACING_METAL_H
