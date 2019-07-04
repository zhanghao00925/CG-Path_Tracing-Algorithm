//
// Created by Hao on 2019/2/27.
//

#include "../core/core.h"
#include "../core/ObjLoader.h"
#include "../samplers/UniformSampler.h"
#include "../integrators/integrators.h"

int main(int argc, char **argv) {
    // Read args
    RenderTask renderTask;
    if (!ArgParser::ParseArg(argc, argv, renderTask)) {
        // Invalid Input
        std::cerr << "Invalid input, please read README.md " << endl;
        return 1;
    }
    // Load Object
    int nx = renderTask.nx;
    int ny = renderTask.ny;
    int ns = renderTask.ns;
    int depth = renderTask.depth;
    // Load Model
    ObjLoader objloader;
    shared_ptr<Scene> scene = objloader.loadObj(renderTask.scene_name);

    // Set Camera
    vec3 lookat = renderTask.look_at;
    vec3 lookfrom = renderTask.look_from;
    float fov = renderTask.fov;
    float dist_to_focus = 10;
    float aperture = 0;

    shared_ptr<Camera> camera = make_shared<Camera>(lookfrom, lookat, vec3(0, 1, 0), fov, float(nx) / float(ny),
                                                    aperture, dist_to_focus, 0.0, 1.0);
    shared_ptr<Sampler> sampler = make_shared<UniformSampler>();
    shared_ptr<Film> film = make_shared<Film>(renderTask.output_name, nx, ny);

    shared_ptr<Integrator> integrator = make_shared<MisIntegrator>(ns, depth);
    integrator->Render(sampler, camera, scene, film);

    cout << "Render finished" << endl;
    return 0;
}
