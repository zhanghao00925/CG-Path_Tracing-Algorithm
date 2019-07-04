//
// Created by hao on 3/27/19.
//

#ifndef PATH_TRACING_MIXINTEGRATOR_H
#define PATH_TRACING_MIXINTEGRATOR_H

#include "../core/core.h"

class MixIntegrator : public Integrator {
public:
    explicit MixIntegrator(int _ns, int _max_depth = 8) : Integrator(_ns, _max_depth) {}

    vec3 color(const Ray &r, shared_ptr<Scene> scene, int depth) {
        Interaction isect;
        if (scene->shapeList->Intersect(r, 1e-3, FLT_MAX, isect)) {
            ScatterRecord srec;
            vec3 emitted = isect.mat_ptr->emitted(r, isect, isect.u, isect.v, isect.p);
            if (depth < max_depth && isect.mat_ptr->scatter(r, isect, srec)) {
                if (srec.is_specular) {
                    return srec.attenuation * color(srec.specular_ray, scene, depth + 1);
                } else if (srec.is_medium) {
                    Ray scattered = Ray(isect.p, srec.pdf_ptr->generate(), r.time());
                    float pdf_val = srec.pdf_ptr->value(scattered.direction());
                    // Scattering_pdf Term include cosine
                    return emitted + srec.attenuation * isect.mat_ptr->scattering_pdf(r, isect, scattered) *
                                     color(scattered, scene, depth + 1) / pdf_val;
                } else {
                    // Mixture importance sampling
//                    MixturePDF pdf(srec.pdf_ptr, make_shared<ShapePDF>(scene->lightList, isect.p));
                    ShapePDF pdf(scene->lightList, isect.p);
                    Ray scattered = Ray(isect.p, pdf.generate(), r.time());
                    float pdf_val = pdf.value(scattered.direction());
                    // Scattering_pdf Term include cosine
                    return emitted + srec.attenuation * isect.mat_ptr->scattering_pdf(r, isect, scattered) *
                                     color(scattered, scene, depth + 1) / pdf_val;
                }
            } else {
                return emitted;
            }
        } else {
            return vec3(0, 0, 0);
        }
    }

};


//#ifdef ENVOIR
// HDR map
//int tnx, tny, tnn;
//unsigned char *tex_data = stbi_load("../assets/textures/winter_river_1k.hdr", &tnx, &tny, &tnn, 0);
//envior = make_shared<Sphere>(vec3(0), 1, make_shared<Mapping>(make_shared<ImageTexture>(tex_data, tnx, tny)));
//ScatterRecord srec;
//        Ray ray = Ray(vec3(0), r.direction(), r.time());
//        envior->Intersect(ray, 0.0001, FLT_MAX, isect);
//        isect.mat_ptr->scatter(ray, isect, srec);
//        return srec.attenuation;
//#endif

#endif //PATH_TRACING_MIXINTEGRATOR_H
