//
// Created by hao on 3/8/19.
//

#ifndef PATH_TRACING_MICROFACETBRDF_H
#define PATH_TRACING_MICROFACETBRDF_H

#include "../core/core.h"

class MicrofacetBRDF : public Material {
public:
    explicit MicrofacetBRDF(shared_ptr<Texture> _albedo, float _roughness, NDF_TYPE _ndf_type = NDF_TYPE::GGX)
            : albedo(_albedo), ndf_type(_ndf_type) {
        roughness = glm::clamp(_roughness, 0.002f, 1.0f);
    }

    virtual vec3 scattering_pdf(const Ray &r_in, const Interaction &isec, const Ray &scattered) const {
        vec3 l = normalize(scattered.direction());
        float cosine = dot(isec.n, l);
        if (cosine < 0)
            return vec3(0.0f);
        vec3 v = normalize(-r_in.direction());
        vec3 h = normalize(l + v);
        vec3 F0 = vec3(0.8);
        vec3 F = FresnelSchlick(dot(h, v), F0);
        float G = GGX_PartialGeometryTerm(v, isec.n, h, roughness);
        float NDF;
        switch (ndf_type) {
            case NDF_TYPE::GGX:
                NDF = GGXDistribution(dot(isec.n, h), roughness);
                break;
            case NDF_TYPE::Beckmann :
                NDF = BeckmannDistribution(dot(isec.n, h), roughness);
                break;
            case NDF_TYPE::Blinn :
                NDF = BlinnDistribution(dot(isec.n, h), roughness);
                break;
        }
        vec3 nominator = NDF * G * F;
        float denominator = 4.0 * dot(isec.n, v); // Remove cosine cause nominator don't multi one
        return nominator / denominator;
    }

    virtual bool scatter(const Ray &r_in, const Interaction &isec, ScatterRecord &srec) const {
        srec.is_specular = false;
        srec.attenuation = albedo->Evaluate(isec.u, isec.v, isec.p);
        srec.pdf_ptr = make_shared<BRDFPDF>(r_in, isec, roughness, ndf_type);
        return true;
    }

    shared_ptr<Texture> albedo;
    float roughness;
    NDF_TYPE ndf_type;
};

#endif //PATH_TRACING_BRDF_H
