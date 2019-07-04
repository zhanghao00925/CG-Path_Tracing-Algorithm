//
// Created by hao on 2/27/19.
//

#ifndef PATH_TRACING_MATERIAL_H
#define PATH_TRACING_MATERIAL_H

#include "Ray.h"
#include "Interaction.h"

vec3 random_in_unit_sphere();

vec3 random_up_hemisphere_direction();

vec3 random_on_triangle();

vec3 reflect(const vec3 &v, const vec3 &n);

bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted);

bool refract(const vec3 &v, const vec3 &n, float ni_over_nt);

float schlick(float cosine, float ref_idx);

inline float chiGGX(float v) {
    return v > 0 ? 1 : 0;
}

inline float GGX_PartialGeometryTerm(vec3 v, vec3 n, vec3 h, float alpha) {
    float VoH2 = dot(v, h);
    float chi = chiGGX(VoH2 / dot(v, n));
    VoH2 = VoH2 * VoH2;
    float tan2 = (1 - VoH2) / VoH2;
    return (chi * 2) / (1 + sqrt(1 + alpha * alpha * tan2));
}

inline vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0f - F0) * float(pow(1.0 - cosTheta, 5.0));
}

inline vec3 random_cosine_direction() {
    float r1 = drand48();
    float r2 = drand48();
    float z = sqrt(1 - r2);
    float phi = 2 * M_PI * r1;
    float x = cos(phi) * 2 * sqrt(r2);
    float y = sin(phi) * 2 * sqrt(r2);
    return vec3(x, y, z);
}

struct ScatterRecord {
    Ray specular_ray;
    bool is_specular = false;
    bool is_medium = false;
    vec3 attenuation;
    shared_ptr<class PDF> pdf_ptr;
};

class ONB {
public:
    ONB() {}

    inline vec3 operator[](int i) const { return axis[i]; }

    vec3 u() const { return axis[0]; }

    vec3 v() const { return axis[1]; }

    vec3 w() const { return axis[2]; }

    vec3 local(float a, float b, float c) const { return a * u() + b * v() + c * w(); }

    vec3 local(const vec3 &a) const { return a.x * u() + a.y * v() + a.z * w(); }

    void build_from_w(const vec3 &n) {
        axis[2] = normalize(n);
        vec3 a;
        if (fabs(w().x) > 0.9)
            a = vec3(0, 1, 0);
        else
            a = vec3(1, 0, 0);
        axis[1] = normalize(cross(w(), a));
        axis[0] = cross(w(), v());
    }

    vec3 axis[3];
};

class Material {
public:
    virtual bool
    scatter(__attribute__((unused))  const Ray &r_in, __attribute__((unused))  const Interaction &isec,
            __attribute__((unused))  ScatterRecord &srec) const {
        return false;
    }

    virtual vec3
    scattering_pdf(__attribute__((unused))  const Ray &r_in, __attribute__((unused))  const Interaction &isect,
                   __attribute__((unused))  const Ray &scattered) const {
        return vec3(false);
    }

    virtual vec3 emitted(__attribute__((unused))  const Ray &r_in, __attribute__((unused))  const Interaction &isect,
                         __attribute__((unused))  float u, __attribute__((unused))  float v,
                         __attribute__((unused)) const vec3 &p) const {
        return vec3(0, 0, 0);
    }

    virtual float alpha_channel(__attribute__((unused)) const Interaction &isect) {
        return 1.0;
    }
};


#endif //PATH_TRACING_MATERIAL_H
