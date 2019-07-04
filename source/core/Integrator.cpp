//
// Created by hao on 3/27/19.
//

#include "Integrator.h"
#include "core.h"

#define JITTERED 

void Integrator::Render(shared_ptr<Sampler> sampler, shared_ptr<Camera> camera,
                  shared_ptr<Scene> scene, shared_ptr<Film> film) {
    int nx = film->nx, ny = film->ny;
    for (int y = ny - 1; y >= 0; y--) {
        for (int x = 0; x < nx; x++) {
            printf("\rProgressing : %.2lf%%.", float((ny - 1 - y) * nx + x) / float(nx * ny) * 100);
            float r(0), g(0), b(0);
#ifndef JITTERED
            int real_ns = ns;
#else
            int spx = int(sqrt(ns));
            int spy = ns / spx;
            int real_ns = spx * spy;
            float dx = 1.0 / float(spx);
            float dy = 1.0 / float(spy);
#endif

#ifdef USE_OPENMP
#pragma omp parallel for  reduction(+:r, g, b)
#endif
            for (int s = 0; s < real_ns; s++) {
                vec2 sample = sampler->Get2D();
#ifndef JITTERED
                float u = (x + sample.x) / float(nx);
                float v = (y + sample.y) / float(ny);
#else
                int delta_y = s / spx;
                int delta_x = s % spx;
                float u = float(x + delta_x * dx + sample.x * dx) / float(nx);
                float v = float(y + delta_y * dy + sample.y * dy) / float(ny);
#endif
                Ray ray = camera->GenerateRay(u, v);
                vec3 result_color = deNan(color(ray, scene, 0));
                r += result_color.r;
                g += result_color.g;
                b += result_color.b;
            }
            vec3 col(r, g, b);
            col /= float(ns);
            col = clamp(sqrt(col), 0.0f, 1.0f); // GAMMA
//            col = clamp(ACESToneMapping(col, 1.0f), 0.0f, 1.0f); // ACES Tone Mapping
            int ir = int(255.99 * col.r);
            int ig = int(255.99 * col.g);
            int ib = int(255.99 * col.b);
            film->Set((ny - 1 - y) * nx + nx - 1 - x, ir, ig, ib);
        }
        // film->WriteImage();
    }
    film->WriteImage();
    return;
}
