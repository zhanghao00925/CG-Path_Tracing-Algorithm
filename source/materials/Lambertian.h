//
// Created by hao on 2/27/19.
//

#ifndef PATH_TRACING_LAMBERTIAN_H
#define PATH_TRACING_LAMBERTIAN_H

#include "../core/core.h"

class Lambertian : public Material {
public:
    explicit Lambertian(shared_ptr<Texture> _albedo) : albedo(_albedo) {}

    virtual vec3 scattering_pdf(__attribute__((unused)) const Ray &r_in, const Interaction &isec, const Ray &scattered) const {
        float cosine = dot(isec.n, normalize(scattered.direction()));
        if (cosine < 0)
            cosine = 0;
        return vec3(cosine / M_PI);
    }

    virtual bool
    scatter(__attribute__((unused)) const Ray &r_in, const Interaction &isec, ScatterRecord &srec) const {
        srec.is_specular = false;
        srec.attenuation = albedo->Evaluate(isec.u, isec.v, isec.p);
        srec.pdf_ptr = make_shared<CosinePDF>(isec.n);
        return true;
    }

    virtual float alpha_channel(const Interaction &isect) {
        return albedo->EvaluateAlpha(isect.u, isect.v, isect.p);
    }
    shared_ptr<Texture> albedo;
};


#endif //PATH_TRACING_LAMBERTIAN_H
