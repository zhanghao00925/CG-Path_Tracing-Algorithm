//
// Created by hao on 3/6/19.
//

#ifndef PATH_TRACING_FILM_H
#define PATH_TRACING_FILM_H

#include "core_std.h"
#include "../ext/ext.h"

enum ToneMappingMethod {
    GAMMA,
    ACES
};

class Film {
public:
    // Film Public Methods
    Film(const std::string &_filename, int _nx, int _ny)
            : filename(_filename), nx(_nx), ny(_ny) {
        pixels = new unsigned char[nx * ny * 3];
        Clear();
    }

    void Set(int index, int r, int g, int b) {
        assert(index < nx * ny);
        pixels[index * 3 + 0] = r;
        pixels[index * 3 + 1] = g;
        pixels[index * 3 + 2] = b;
    }

    void Clear() {
        memset(pixels, 0, sizeof(unsigned char) * nx * ny);
    }

    void WriteImage() {
        stbi_write_bmp(filename.c_str(), nx, ny, 3, pixels);
    }

    ~Film() {
        delete[] pixels;
    }

    // Film Public Data
    const std::string filename;
    int nx, ny;
    unsigned char *pixels;
};

inline vec3 ACESToneMapping(vec3 color, float adapted_lum) {
    const float A = 2.51f;
    const float B = 0.03f;
    const float C = 2.43f;
    const float D = 0.59f;
    const float E = 0.14f;

    color *= adapted_lum;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

inline vec3 Gamma(vec3 color, float gamma) {
    return glm::pow(color, vec3(1.0 / gamma));
}


#endif //PATH_TRACING_FILM_H
