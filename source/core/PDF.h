//
// Created by hao on 3/2/19.
//

#ifndef PATH_TRACING_PDF_H
#define PATH_TRACING_PDF_H

#include "Material.h"
#include "Shape.h"

using namespace std;

class PDF {
public:
    virtual float value(const vec3 &direction) const = 0;

    virtual vec3 generate() const = 0;
};

class CosinePDF : public PDF {
public:
    CosinePDF(const vec3 &w) {
        uvw.build_from_w(w);
    }

    virtual float value(const vec3 &direction) const {
        float cosine = dot(normalize(direction), uvw.w());
        if (cosine > 0)
            return cosine / M_PI;
        else
            return 0;
    }

    virtual vec3 generate() const {
        return uvw.local(random_cosine_direction());
    }

    ONB uvw;
};

enum NDF_TYPE {
    GGX,
    Beckmann,
    Blinn
};

inline float GGXDistribution(float cosTheta, float alpha) {
    return (alpha * alpha) / (M_PI * pow((alpha * alpha - 1.0) * cosTheta * cosTheta + 1.0, 2.0));
}

inline vec3 SampleGGX(float alpha) {
    float r1 = drand48();
    float r2 = drand48();
    float phi = 2.0f * M_PI * r1;
    float costheta = sqrt((1.0 - r2) / (r2 * (alpha * alpha - 1) + 1));
    float sintheta = sqrt(1 - costheta * costheta);
    return vec3(cos(phi) * sintheta, sin(phi) * sintheta, costheta);
}

inline float BeckmannDistribution(float cosTheta, float alpha) {
    float sinTheta = sqrt(1 - cosTheta * cosTheta);
    float tangent = sinTheta / cosTheta;
    return (1.0 / (M_PI * pow(alpha, 2.0) * pow(cosTheta, 4.0))) * exp(-1.0 * pow(tangent / alpha, 2));
}

inline vec3 SampleBeckmann(float alpha) {
    float r1 = drand48();
    float r2 = drand48();
    float phi = 2.0f * M_PI * r1;
    float costheta = sqrt(1.0 / (1.0 - alpha * alpha * log(r2)));
    float sintheta = sqrt(1 - costheta * costheta);
    return vec3(cos(phi) * sintheta, sin(phi) * sintheta, costheta);
}

inline float BlinnDistribution(float cosTheta, float alpha) {
    return (1.0 / (M_PI * pow(alpha, 2.0))) * pow(cosTheta, (2.0 / pow(alpha, 2.0) - 2.0));
//    return (alpha + 2.0) / (2.0 * M_PI) * pow(cosTheta, alpha);
}

inline vec3 SampleBlinn(float alpha) {
    float r1 = drand48();
    float r2 = drand48();
    float phi = 2.0f * M_PI * r1;
    float costheta = pow(r2, pow(alpha, 2) / 2.0);
//    float costheta = pow(r2, 1.0f / (alpha + 2.0f));
    float sintheta = sqrt(1 - costheta * costheta);
    return vec3(cos(phi) * sintheta, sin(phi) * sintheta, costheta);
}

// Probability of reflection
inline float FresnelSchlick(float cosTheta, float F0) {
    return F0 + (1.0f - F0) * float(pow(1.0 - cosTheta, 5.0));
}

class BRDFPDF : public PDF {
public:
    BRDFPDF(const Ray &r_in, const Interaction &isec, float _alpha, NDF_TYPE _ndf_type)
            : alpha(_alpha), ndf_type(_ndf_type) {
        uvw.build_from_w(normalize(isec.n));
        v = normalize(-r_in.direction());
    }

    virtual float value(const vec3 &direction) const {
        vec3 l = normalize(direction);
        vec3 h = normalize(l + v);
        float cosTheta = abs(dot(uvw.w(), h));
        float pdf = 0;
        switch (ndf_type) {
            case NDF_TYPE::GGX :
                pdf = GGXDistribution(cosTheta, alpha);
                break;
            case NDF_TYPE::Beckmann :
                pdf = BeckmannDistribution(cosTheta, alpha);
                break;
            case NDF_TYPE::Blinn :
                pdf = BlinnDistribution(cosTheta, alpha);
                break;
        }
        return pdf * cosTheta / (4.0f * abs(dot(l, h)));
    }

    virtual vec3 generate() const {
        vec3 h;
        switch (ndf_type) {
            case NDF_TYPE::GGX :
                h = normalize(uvw.local(SampleGGX(alpha)));
                break;
            case NDF_TYPE::Beckmann :
                h = normalize(uvw.local(SampleBeckmann(alpha)));
                break;
            case NDF_TYPE::Blinn :
                h = normalize(uvw.local(SampleBlinn(alpha)));
                break;
        }
        if (dot(v, h) > 0) {
            return glm::reflect(-v, h);
        } else {
            return glm::reflect(-v, -h);
        }
    }

    ONB uvw;
    vec3 v;
    float alpha;
    NDF_TYPE ndf_type;
};

class BSDFPDF : public PDF {
public:
    // n1 is index of the new medium
    BSDFPDF(const Ray &r_in, const Interaction &isec, float _alpha, float _index_v, float _index_l,
            NDF_TYPE _ndf_type)
            : alpha(_alpha), index_v(_index_v), index_l(_index_l), ndf_type(_ndf_type) {
        v = normalize(-r_in.direction());
        if (dot(v, isec.n) > 0) {
            uvw.build_from_w(normalize(isec.n));
        } else {
            uvw.build_from_w(normalize(-isec.n));
        }
        R0 = pow((index_l - index_v) / (index_l + index_v), 2);
    }

    virtual float value(const vec3 &direction) const {
        vec3 l = normalize(direction);
        vec3 n = normalize(uvw.w());
        vec3 h_t = normalize(-index_v * v - index_l * l);
        vec3 h_r = normalize(l + v);
        float pdf_refraction = 0;
        if (refract(-v, h_t, index_v / index_l)) {
            // refract
            float cosTheta = abs(dot(n, h_t)), pdf_h = 0;
            switch (ndf_type) {
                case NDF_TYPE::GGX :
                    pdf_h = GGXDistribution(cosTheta, alpha);
                    break;
                case NDF_TYPE::Beckmann :
                    pdf_h = BeckmannDistribution(cosTheta, alpha);
                    break;
                case NDF_TYPE::Blinn :
                    pdf_h = BlinnDistribution(cosTheta, alpha);
                    break;
            }
            float refract_prob = 1 - FresnelSchlick(abs(dot(v, h_t)), R0);
            float eta = index_l / index_v;
            const float sIoH = dot(v, h_t);
            const float sVoH = dot(l, h_t);
            float sqrtDenom = sIoH + eta * sVoH;
            float dwh_dwi = eta * eta * abs(dot(v, h_t)) / (sqrtDenom * sqrtDenom);
            if (dwh_dwi > 0) {
                pdf_refraction = pdf_h * cosTheta * dwh_dwi * refract_prob;
            }
        }
        float pdf_reflection = 0;
        // Case reflection
        {
            float cosTheta = abs(dot(n, h_r)), pdf_h = 0;
            switch (ndf_type) {
                case NDF_TYPE::GGX :
                    pdf_h = GGXDistribution(cosTheta, alpha);
                    break;
                case NDF_TYPE::Beckmann :
                    pdf_h = BeckmannDistribution(cosTheta, alpha);
                    break;
                case NDF_TYPE::Blinn :
                    pdf_h = BlinnDistribution(cosTheta, alpha);
                    break;
            }
            float reflect_prob = 0.0f;
            if (refract(-v, h_r, index_v / index_l)) {
                reflect_prob = FresnelSchlick(abs(dot(v, h_r)), R0);
            } else {
                reflect_prob = 1.0f;
            }
            float dwh_dwi = (4.0f * abs(dot(l, h_r)));
            if (dwh_dwi > 0) {
                pdf_reflection = pdf_h * cosTheta / dwh_dwi * reflect_prob;
            }
        }
        return pdf_reflection + pdf_refraction;
    }

    virtual vec3 generate() const {
        vec3 h;
        // Sample normal
        switch (ndf_type) {
            case NDF_TYPE::GGX :
                h = normalize(uvw.local(SampleGGX(alpha)));
                break;
            case NDF_TYPE::Beckmann :
                h = normalize(uvw.local(SampleBeckmann(alpha)));
                break;
            case NDF_TYPE::Blinn :
                h = normalize(uvw.local(SampleBlinn(alpha)));
                break;
        }
        float cosine = dot(v, h);
        if (cosine < 0) {
            h = -h;
        }
        cosine = dot(v, h);
        vec3 refracted;
        float ni_over_nt = index_v / index_l;
        float reflect_prob;
        if (refract(-v, h, ni_over_nt, refracted)) {
            reflect_prob = FresnelSchlick(cosine, R0);
        } else {
            reflect_prob = 1.0;
        }
        if (drand48() < reflect_prob) {
            return reflect(-v, h);;
        } else {
            return refracted;
        }
    }

    ONB uvw;
    vec3 v;
    float alpha, index_v, index_l;
    float R0;
    NDF_TYPE ndf_type;
};

class ShapePDF : public PDF {
public:
    ShapePDF(shared_ptr<Shape> shape_ptr, const vec3 &origin) : o(origin), ptr(shape_ptr) {}

    virtual float value(const vec3 &direction) const {
        return ptr->pdf_value(o, direction);
    }

    virtual vec3 generate() const {
        return ptr->random(o);
    }

    vec3 o;
    shared_ptr<Shape> ptr;
};

class MixturePDF : public PDF {
public:
    MixturePDF(shared_ptr<PDF> _pdf0, shared_ptr<PDF> _pdf1, float _p0 = 0.5, float _p1 = 0.5)
            : pdf0(_pdf0), pdf1(_pdf1), p0(_p0), p1(_p1) {}

    virtual float value(const vec3 &direction) const {
        return p0 * pdf0->value(direction) + p1 * pdf1->value(direction);
    }

    virtual vec3 generate() const {
        if (drand48() < p0) {
            return pdf0->generate();
        } else {
            return pdf1->generate();
        }
    }

    shared_ptr<PDF> pdf0, pdf1;
    float p0, p1;
};

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
    float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}

#endif //PATH_TRACING_PDF_H
