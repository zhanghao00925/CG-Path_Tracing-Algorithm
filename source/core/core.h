//
// Created by Hao on 2019/2/27.
//

#ifndef PATH_TRACING_RT_H
#define PATH_TRACING_RT_H

#include "core_std.h"

#include "../ext/ext.h"

#include "Camera.h"
#include "Film.h"
#include "Filter.h"
#include "Geometry.h"
#include "Integrator.h"
#include "Interaction.h"
#include "Material.h"
#include "Parser.h"
#include "PDF.h"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"
#include "Shape.h"
#include "Texture.h"

inline vec3 deNan(const vec3 &c) {
    vec3 temp = c;
    if (!(temp.x == temp.x)) temp.x = 0;
    if (!(temp.y == temp.y)) temp.y = 0;
    if (!(temp.z == temp.z)) temp.z = 0;
    return temp;
}

#endif
