//
// Created by hao on 3/27/19.
//

#ifndef PATH_TRACING_BRDF_H
#define PATH_TRACING_BRDF_H

#include "../core/core.h"

class BRDF : public Material {
public:
    explicit BRDF(shared_ptr<Texture> _diffuse, shared_ptr<Texture> _specular,
                  float _pDiffuse, float _pSpecular,
                  float _roughness, NDF_TYPE _ndf_type = NDF_TYPE::GGX)
            : diffuse(_diffuse), specular(_specular),
              pDiffuse(_pDiffuse), pSpecular(_pSpecular),
              alpha(_roughness), ndf_type(_ndf_type) {
        alpha = glm::clamp(alpha, 0.0f, 1.0f);
    }

    virtual vec3 scattering_pdf(const Ray &r_in, const Interaction &isec, const Ray &scattered) const {
        vec3 l = normalize(scattered.direction());
        vec3 diffuseRadiance(0), specularRadiance(0);
        float cosine = dot(isec.n, l);
        // Diffuse
        if (cosine < 0)
            return vec3(0.0f);
        if (pDiffuse > 1e-8) {
            diffuseRadiance = vec3(cosine / M_PI) * diffuse->Evaluate(isec.u, isec.v, isec.p);
        }
        // Specular
        if (pSpecular > 1e-8) {
            vec3 v = normalize(-r_in.direction());
            vec3 h = normalize(l + v);
            vec3 F0 = vec3(1.0);
            vec3 F = FresnelSchlick(dot(h, v), F0);
            float G = GGX_PartialGeometryTerm(v, isec.n, h, alpha);
            float NDF = 0.0f;
            switch (ndf_type) {
                case NDF_TYPE::GGX:
                    NDF = GGXDistribution(dot(isec.n, h), alpha);
                    break;
                case NDF_TYPE::Beckmann :
                    NDF = BeckmannDistribution(dot(isec.n, h), alpha);
                    break;
                case NDF_TYPE::Blinn :
                    NDF = BlinnDistribution(dot(isec.n, h), alpha);
                    break;
            }
            float denominator = 4.0 * dot(isec.n, v);
            // Remove cosine cause nominator don't multi one
            if (denominator > 0) {
                specularRadiance = NDF * F * G / denominator * specular->Evaluate(isec.u, isec.v, isec.p);
            }
        }
        // Mix diffuse and specular
        return diffuseRadiance + specularRadiance;
    }

    virtual bool scatter(const Ray &r_in, const Interaction &isec, ScatterRecord &srec) const {
        srec.is_specular = false;
        srec.attenuation = vec3(1.0);
        srec.pdf_ptr = make_shared<MixturePDF>(make_shared<CosinePDF>(isec.n),
                                               make_shared<BRDFPDF>(r_in, isec, alpha, ndf_type),
                                               pDiffuse, pSpecular);
        return true;
    }

    virtual float alpha_channel(const Interaction &isect) {
        return diffuse->EvaluateAlpha(isect.u, isect.v, isect.p);
    }

    shared_ptr<Texture> diffuse;
    shared_ptr<Texture> specular;
    float pDiffuse, pSpecular;
    float alpha;
    NDF_TYPE ndf_type;
};


#endif //PATH_TRACING_BRDF_H
