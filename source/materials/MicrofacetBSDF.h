//
// Created by hao on 3/10/19.
//

#ifndef PATH_TRACING_MICROFACETBSDF_H
#define PATH_TRACING_MICROFACETBSDF_H

#include "../core/core.h"

class MicrofacetBSDF : public Material {
public:
    explicit MicrofacetBSDF(float ri, shared_ptr<Texture> _albedo, float _roughness = 0.01f,
                            NDF_TYPE _ndf_type = NDF_TYPE::GGX)
            :  ref_idx(ri), albedo(_albedo),ndf_type(_ndf_type) {
        roughness = glm::clamp(_roughness, 0.00f, 1.0f);
    }

    virtual vec3 scattering_pdf(const Ray &r_in, const Interaction &isec, const Ray &scattered) const {
        // Pre-compute
        vec3 l = normalize(scattered.direction());
        vec3 v = normalize(-r_in.direction());
        vec3 n;
        float index_v, index_l;
        if (dot(v, isec.n) > 0) {
            // From air to medium
            n = normalize(isec.n);
            index_v = 1;
            index_l = ref_idx;
        } else {
            n = -normalize(isec.n);
            index_v = ref_idx;
            index_l = 1;
        }
        vec3 h_t = -normalize(-index_v * v - index_l * l);
        if (dot(n, h_t) < 0) {
            h_t = -h_t;
        }
        float R0 = pow((index_l - index_v) / (index_l + index_v), 2);
        if (dot(l, n) < 0 && dot(l, -v) > 0) {
            // refraction
            float refraction_Contribution;
            float pdf_refraction = 0;
            float cosTheta = abs(dot(n, h_t));
            switch (ndf_type) {
                case NDF_TYPE::GGX :
                    pdf_refraction = GGXDistribution(cosTheta, roughness);
                    break;
                case NDF_TYPE::Beckmann :
                    pdf_refraction = BeckmannDistribution(cosTheta, roughness);
                    break;
                case NDF_TYPE::Blinn :
                    pdf_refraction = BlinnDistribution(cosTheta, roughness);
                    break;
            }
            float F = FresnelSchlick(dot(h_t, v), R0);
            float G = GGX_PartialGeometryTerm(v, n, h_t, roughness);
            float eta = index_l / index_v;
            float sqrtDenom = dot(v, h_t) + eta * dot(l, h_t);
            float header = abs(dot(v, h_t) * dot(l, h_t)) / abs(dot(v, n) * dot(l, n));
            refraction_Contribution =
                    header * abs((1.0 - F) * G * pdf_refraction * eta * eta / (sqrtDenom * sqrtDenom)) *
                    abs(dot(l, n));
            return vec3(refraction_Contribution);
        }
        vec3 h_r = normalize(l + v);
        if (dot(l, n) > 0) {
            // reflection
            float cosTheta = abs(dot(n, h_r));
            float pdf_reflection = 0;
            switch (ndf_type) {
                case NDF_TYPE::GGX :
                    pdf_reflection = GGXDistribution(cosTheta, roughness);
                    break;
                case NDF_TYPE::Beckmann :
                    pdf_reflection = BeckmannDistribution(cosTheta, roughness);
                    break;
                case NDF_TYPE::Blinn :
                    pdf_reflection = BlinnDistribution(cosTheta, roughness);
                    break;
            }
            float F = 0.0;
//            if (refract(-v, h_r, index_v / index_l)) {
            F = FresnelSchlick(abs(dot(v, h_r)), R0);
//            } else {
//                F = 1.0f;
//            }
            float G = GGX_PartialGeometryTerm(v, n, h_r, roughness);
            float nominator = pdf_reflection * G * F;
            float denominator = 4.0 * dot(n, v); // Remove cosine cause nominator don't multi one
            return vec3(nominator / denominator);
        }
        return vec3(0);
    }

    virtual bool scatter(const Ray &r_in, const Interaction &isec, ScatterRecord &srec) const {
        srec.is_specular = false;
        srec.is_medium = true;
        srec.attenuation = vec3(1.0f);
        srec.attenuation = albedo->Evaluate(isec.u, isec.v, isec.p);
        if (dot(-r_in.direction(), isec.n) > 0) {
            srec.pdf_ptr = make_shared<BSDFPDF>(r_in, isec, roughness, 1.0, ref_idx, ndf_type);

        } else {
            srec.pdf_ptr = make_shared<BSDFPDF>(r_in, isec, roughness, ref_idx, 1.0, ndf_type);
        }
        return true;
    }

    float ref_idx;
    shared_ptr<Texture> albedo;
    float roughness;
    NDF_TYPE ndf_type;
};


#endif //PATH_TRACING_BSDF_H
