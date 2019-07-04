//
// Created by hao on 3/27/19.
//

#ifndef PATH_TRACING_MISINTEGRATOR_H
#define PATH_TRACING_MISINTEGRATOR_H

#include "../core/core.h"

class MisIntegrator : public Integrator {
public:
    explicit MisIntegrator(int _ns, int _max_depth = 6) : Integrator(_ns, _max_depth) {}


    virtual vec3 color(const Ray &r, shared_ptr<Scene> scene, int depth) {
        static const float epsilon = 1e-4;
        Interaction isect;
        vec3 L = vec3(0, 0, 0);
        if (scene->shapeList->Intersect(r, 0.0001, FLT_MAX, isect)) {
            // Hit something
            ScatterRecord srec;
            vec3 emitted = isect.mat_ptr->emitted(r, isect, isect.u, isect.v, isect.p);
            if (depth == 0 || r.bIsShadowRay) {
                L += emitted;
            }
            if (!r.bIsShadowRay && depth < max_depth && isect.mat_ptr->scatter(r, isect, srec)) {
                if (srec.is_specular) {
                    // Calculate light
                    Ray scattered_bsdf = srec.specular_ray;
                    scattered_bsdf.bIsShadowRay = true;
                    vec3 radiance = srec.attenuation * color(scattered_bsdf, scene, 0);
                    L += radiance + srec.attenuation * color(srec.specular_ray, scene, depth + 1);
                } else if (srec.is_medium) {
                    Ray scattered_bsdf = Ray(isect.p, srec.pdf_ptr->generate(), r.time(), true);
                    vec3 radiance = srec.attenuation * color(scattered_bsdf, scene, 0);
                    L += radiance;
                    //
                    scattered_bsdf.bIsShadowRay = false;
                    float pdf_val = srec.pdf_ptr->value(scattered_bsdf.direction());
                    if (pdf_val > epsilon) {
                        L +=srec.attenuation * isect.mat_ptr->scattering_pdf(r, isect, scattered_bsdf) *
                           color(scattered_bsdf, scene, depth + 1) / pdf_val;
                    }
                } else {
                    // Direct lighting
                    vec3 radiance(0);
                    float pdf_bsdf(0), pdf_light(0);
                    float weight_l(0), weight_bsdf(0);
                    vec3 lightEmitted(0.0);
                    ShapePDF light_pdf(scene->lightList, isect.p);

                    // Sample a point according to the light
                    Ray scattered_l = Ray(isect.p, light_pdf.generate(), r.time(), true);
                    pdf_light = light_pdf.value(scattered_l.direction());
                    if (pdf_light > epsilon) {
                        pdf_bsdf = srec.pdf_ptr->value(scattered_l.direction());
                        if (pdf_bsdf > epsilon) {
                            weight_l = PowerHeuristic(1, pdf_light, 1, pdf_bsdf);
                            lightEmitted = color(scattered_l, scene, 0);
                            if (length(lightEmitted) > epsilon) {
                                // Not hidden
                                radiance +=
                                        srec.attenuation * isect.mat_ptr->scattering_pdf(r, isect, scattered_l) *
                                        lightEmitted * weight_l / pdf_light;
                            }
                        }
                    }
                    // Sample a point according to the bsdf
                    Ray scattered_bsdf = Ray(isect.p, srec.pdf_ptr->generate(), r.time(), true);
                    pdf_bsdf = srec.pdf_ptr->value(scattered_bsdf.direction());
                    if (pdf_bsdf > epsilon) {
                        pdf_light = light_pdf.value(scattered_bsdf.direction());
                        if (pdf_light > epsilon) {
                            weight_bsdf = PowerHeuristic(1, pdf_bsdf, 1, pdf_light);
                            lightEmitted = color(scattered_bsdf, scene, 0);
                            if (length(lightEmitted) > epsilon) {
                                // Not hidden
                                radiance +=
                                        srec.attenuation * isect.mat_ptr->scattering_pdf(r, isect, scattered_bsdf) *
                                        lightEmitted * weight_bsdf / pdf_bsdf;
                            }
                        }
                    }
                    L += radiance;

                    // Indirect lighting
                    scattered_bsdf = Ray(isect.p, srec.pdf_ptr->generate(), r.time());
                    pdf_bsdf = srec.pdf_ptr->value(scattered_bsdf.direction());
                    // Scattering_pdf Term include cosine
                    if (pdf_bsdf > epsilon) {
                        L += srec.attenuation * isect.mat_ptr->scattering_pdf(r, isect, scattered_bsdf) *
                             color(scattered_bsdf, scene, depth + 1) / pdf_bsdf;
                    }
                }
            }
        }
        return L;
    }

};


#endif //PATH_TRACING_MISINTEGRATOR_H
