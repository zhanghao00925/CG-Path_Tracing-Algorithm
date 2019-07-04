//
// Created by hao on 2/27/19.
//

#ifndef PATH_TRACING_SCENE_H
#define PATH_TRACING_SCENE_H

#include <limits>

#include "Ray.h"
#include "Interaction.h"
#include "Shape.h"

class Scene {
public:
    Scene() = delete;

    Scene(shared_ptr<Shape> _shapeList, shared_ptr<Shape> _lightList, shared_ptr<Shape> _sampleList)
            : shapeList(_shapeList), lightList(_lightList), sampleList(_sampleList) {}

    shared_ptr<Shape> shapeList, lightList, sampleList;
};


#endif //PATH_TRACING_SCENE_H
