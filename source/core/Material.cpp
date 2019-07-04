//
// Created by hao on 2/27/19.
//

#include "Material.h"

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0f * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while (dot(p, p) >= 1.0);
    return p;
}

vec3 random_up_hemisphere_direction() {
//    // Method 1
//    float u = 2 * drand48() - 1;
//    float phi = 2 * M_PI * drand48();
//    float z = u>0?u:-u;
//    float x = cos(phi) * sqrt(1 - z * z);
//    float y = sin(phi) * sqrt(1 - z * z);
//    return vec3(x, y, z);
    vec3 p;
    do {
        p = 2.0f * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while (dot(p, p) >= 1.0);
    if (p.z < 0)
        p.z = -p.z;
    return normalize(p);
}

vec3 random_on_triangle() {
    float alpha = drand48();
    float beta = drand48();
    if (alpha + beta > 1) {
        alpha = 1 - alpha;
        beta = 1 - beta;
    }
    return vec3(alpha, beta, 1 - alpha - beta);
}

vec3 reflect(const vec3 &v, const vec3 &n) {
    return v - 2.0f * dot(v, n) * n;
}

float schlick(float cosine, float ref_idx) {
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted) {
    vec3 uv = normalize(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    } else
        return false;
}

bool refract(const vec3 &v, const vec3 &n, float ni_over_nt) {
    float dt = dot(normalize(v), normalize(n));
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0) {
        return true;
    } else {
        return false;
    }
}