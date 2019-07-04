//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_IMAGETEXTURE_H
#define PATH_TRACING_IMAGETEXTURE_H

#include "../core/core.h"

class ImageTexture : public Texture {
public:
    ImageTexture() = default;

    ImageTexture(unsigned char *pixels, int _nx, int _ny, int _channel)
            : data(pixels), nx(_nx), ny(_ny), channel(_channel) {}

    ~ImageTexture() {
        delete[] data;
    }

    virtual vec3 Evaluate(float u, float v, __attribute__((unused)) const vec3 &p) const override {
        int i = (u) * nx;
        int j = (1 - v) * ny - 0.001;
        if (i < 0) i = 0;
        if (j < 0) j = 0;
        if (i > nx - 1) i = nx - 1;
        if (j > ny - 1) j = ny - 1;
        float r = int(data[channel * i + channel * nx * j]) / 255.0;
        float g = int(data[channel * i + channel * nx * j + 1]) / 255.0;
        float b = int(data[channel * i + channel * nx * j + 2]) / 255.0;
        return vec3(r, g, b);
    }

    virtual float EvaluateAlpha(float u, float v, __attribute__((unused)) const vec3 &p) {
        if (channel == 4) {
            int i = (u) * nx;
            int j = (1 - v) * ny - 0.001;
            if (i < 0) i = 0;
            if (j < 0) j = 0;
            if (i > nx - 1) i = nx - 1;
            if (j > ny - 1) j = ny - 1;
            return int(data[channel * i + channel * nx * j + 3]) / 255.0;
        } else {
            return 1.0;
        }
    }

    unsigned char *data;
    int nx, ny, channel;
};


#endif //PATH_TRACING_IMAGETEXTURE_H
