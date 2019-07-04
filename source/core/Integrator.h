//
// Created by hao on 3/27/19.
//

#ifndef PATH_TRACING_INTEGRATOR_H
#define PATH_TRACING_INTEGRATOR_H

#include "core_std.h"
#include "../ext/ext.h"

class Sampler;
class Scene;
class Ray;
class Film;
class Camera;

class Integrator {
public:
    explicit Integrator(int _ns, int _max_depth = 8) : ns(_ns), max_depth(_max_depth) {}

    virtual void Render(shared_ptr<Sampler> sampler, shared_ptr<Camera> camera,
                        shared_ptr<Scene> scene, shared_ptr<Film> film);

    virtual vec3 color(const Ray &r, shared_ptr<Scene> scene, int depth) = 0;

    int ns, max_depth;
};


#endif //PATH_TRACING_INTEGRATOR_H
