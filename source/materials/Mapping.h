//
// Created by hao on 3/25/19.
//

#ifndef PATH_TRACING_MAPPING_H
#define PATH_TRACING_MAPPING_H


#include "../core/core.h"

class Mapping : public Material {
public:
    explicit Mapping(shared_ptr<Texture> _albedo) : albedo(_albedo) {}

    virtual vec3
    scattering_pdf(__attribute__((unused)) const Ray &r_in, __attribute__((unused)) const Interaction &isec,
                   __attribute__((unused))  const Ray &scattered) const {
        return vec3(0);
    }

    virtual bool
    scatter(__attribute__((unused)) const Ray &r_in, const Interaction &isec, ScatterRecord &srec) const {
        srec.is_specular = false;
        srec.attenuation = albedo->Evaluate(isec.u, isec.v, isec.p);
        srec.pdf_ptr = nullptr;
        return true;
    }

    shared_ptr<Texture> albedo;
};


#endif //PATH_TRACING_MAPPING_H
