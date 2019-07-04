//
// Created by hao on 2/27/19.
//

#ifndef PATH_TRACING_INTERACTION_H
#define PATH_TRACING_INTERACTION_H

#include <memory>

using namespace std;

#include "glm/glm.hpp"

using namespace glm;

class Material;

class Interaction {
public:
    float u, v;
    float t;
    vec3 p;
    vec3 n;
    shared_ptr<Material> mat_ptr;
};


#endif //PATH_TRACING_INTERACTION_H
