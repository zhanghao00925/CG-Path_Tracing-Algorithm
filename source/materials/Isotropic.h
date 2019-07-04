//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_ISOTROPIC_H
#define PATH_TRACING_ISOTROPIC_H

#include "../core/core.h"

class Isotropic : public Material {
public:
    Isotropic(shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(const Ray &r_in, const Interaction &isec, ScatterRecord &srec) const {
        srec.specular_ray = Ray(isec.p, random_in_unit_sphere(), r_in.t);
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = albedo->Evaluate(isec.u, isec.v, isec.p);;
        return true;
    }
    shared_ptr<Texture> albedo;
};

#endif //PATH_TRACING_ISOTROPIC_H
