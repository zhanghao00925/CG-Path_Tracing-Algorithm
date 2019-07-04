//
// Created by hao on 2/28/19.
//

#ifndef PATH_TRACING_DIELECTRIC_H
#define PATH_TRACING_DIELECTRIC_H

#include "../core/core.h"

class Dielectric : public Material {
public:
    explicit Dielectric(float ri, shared_ptr<Texture> _diffuse)
            : ref_idx(ri), diffuse(_diffuse)  {}

    virtual bool scatter(const Ray &r_in, const Interaction &isec, ScatterRecord &srec) const {
        srec.is_specular = true;
        srec.pdf_ptr = 0;
        srec.attenuation = diffuse->Evaluate(isec.u, isec.v, isec.p);
        vec3 outward_normal;
        vec3 h = isec.n;
        vec3 reflected = reflect(r_in.direction(), h);
        vec3 refracted;
        float ni_over_nt;
        float reflect_prob;
        float cosine;
        if (dot(r_in.direction(), h) > 0) {
            outward_normal = -h;
            ni_over_nt = ref_idx;
            cosine = dot(r_in.direction(), h) / length(r_in.direction());
        } else {
            outward_normal = h;
            ni_over_nt = 1.0f / ref_idx;
            cosine = -dot(r_in.direction(), h) / length(r_in.direction());
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
            reflect_prob = schlick(cosine, ref_idx);
        } else {
            reflect_prob = 1.0;
        }
        if (drand48() < reflect_prob) {
            srec.specular_ray = Ray(isec.p, reflected, r_in.t);
        } else {
            srec.specular_ray = Ray(isec.p, refracted, r_in.t);
        }
        return true;
    }


    float ref_idx;
    shared_ptr<Texture> diffuse;
};


#endif //PATH_TRACING_DIELECTRIC_H
