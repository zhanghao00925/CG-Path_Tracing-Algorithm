//
// Created by hao on 3/27/19.
//

#ifndef PATH_TRACING_DIRECTINTEGRATOR_H
#define PATH_TRACING_DIRECTINTEGRATOR_H

#include "../core/core.h"

class DirectIntegrator : public Integrator {
public:
    explicit DirectIntegrator(int _ns, __attribute__((unused)) int depth) : Integrator(_ns, 1) {}

    virtual vec3 color(const Ray &r, shared_ptr<Scene> scene, int depth) {
        Interaction isect;
        vec3 L = vec3(0, 0, 0);
        if (scene->shapeList->Intersect(r, 0.0001, FLT_MAX, isect)) {
            // Hit something
            ScatterRecord srec;
            vec3 emitted = isect.mat_ptr->emitted(r, isect, isect.u, isect.v, isect.p);
            if (depth == 0) {
                L += emitted;
            }
            if (!r.bIsShadowRay) {
                if (depth < max_depth && isect.mat_ptr->scatter(r, isect, srec)) {
                    if (srec.is_specular) {
                        // Calculate light
                        Ray scattered_bsdf = srec.specular_ray;
                        scattered_bsdf.bIsShadowRay = true;
                        vec3 radiance = srec.attenuation * color(scattered_bsdf, scene, 0);
                        return L + radiance;
                    } else if (srec.is_medium) {
                        Ray scattered_bsdf = Ray(isect.p, srec.pdf_ptr->generate(), r.time(), true);
                        vec3 radiance = srec.attenuation * color(scattered_bsdf, scene, 0);
                        return L + radiance;
                    } else {
                        // Direct lighting
                        vec3 radiance(0);
                        float pdf_bsdf = 0, pdf_light = 0;
                        float weight_l, weight_bsdf;
                        ShapePDF light_pdf(scene->lightList, isect.p);

                        // Sample a point according to the light
                        Ray scattered_l = Ray(isect.p, light_pdf.generate(), r.time(), true);
                        pdf_light = light_pdf.value(scattered_l.direction());
//                        if (pdf_light > 1e-6) {
                            pdf_bsdf = srec.pdf_ptr->value(scattered_l.direction());
                            weight_l = PowerHeuristic(1, pdf_light, 1, pdf_bsdf);
                            vec3 lightEmitted = color(scattered_l, scene, 0);
                            if (length(lightEmitted) > 1e-6) {
                                // Not hidden
                                radiance += srec.attenuation * isect.mat_ptr->scattering_pdf(r, isect, scattered_l) *
                                            lightEmitted * weight_l / pdf_light;
                            }
//                        }

                        // Sample a point according to the bsdf
                        Ray scattered_bsdf = Ray(isect.p, srec.pdf_ptr->generate(), r.time(), true);
                        pdf_bsdf = srec.pdf_ptr->value(scattered_bsdf.direction());
                        pdf_light = light_pdf.value(scattered_bsdf.direction());
                        if (pdf_light > 0) {
                            weight_bsdf = PowerHeuristic(1, pdf_bsdf, 1, pdf_light);
                            vec3 lightEmitted = color(scattered_l, scene, 0);
                            if (length(lightEmitted) > 0) {
                                // Not hidden
                                radiance += srec.attenuation * isect.mat_ptr->scattering_pdf(r, isect, scattered_l) *
                                            lightEmitted * weight_bsdf / pdf_bsdf;
                            }
                        }
                        return L + radiance;
                    }
                }
            }
        }
        return L;
    }
};


#endif //PATH_TRACING_DIRECTINTEGRATOR_H
