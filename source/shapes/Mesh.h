//
// Created by hao on 3/6/19.
//

#ifndef PATH_TRACING_MESH_H
#define PATH_TRACING_MESH_H


#include "../core/core.h"
#include "../accelerators/BVH.h"

class Mesh : public Shape {
public:
    Mesh() {}

    Mesh(vector<shared_ptr<Shape>> l) : face_list(l) {
        bvh = make_shared<BVHNode>(face_list.begin(), face_list.size(), 0.0, 1.0);
        float totalArea = 0;
        for (auto &triangle :face_list) {
            float area = triangle->surface_area;
            totalArea += area;
            cumulativeAreas.push_back(totalArea);
        }
        surface_area = totalArea;
    }

    virtual bool Intersect(const Ray &r, float tmin, float tmax, Interaction &rec) const {
        Interaction isec;
        if (bvh->Intersect(r, tmin, tmax, isec)) {
            rec = isec;
            return true;
        }
        return false;
    }

    virtual bool bounding_box(float t0, float t1, AABB &box) const {
        return bvh->bounding_box(t0, t1, box);
    }

    virtual float pdf_value(const vec3 &o, const vec3 &v) const {
        Interaction rec;
        float pdf = 0;
        float tmin = 1e-4;
        while (bvh->Intersect(Ray(o, v, 0), tmin, FLT_MAX, rec)) {
            float distance_squared = rec.t * rec.t * dot(v, v);
            float cosine = fabs(dot(v, rec.n) / length(v));
            if (cosine > 0) {
                pdf += distance_squared / (cosine * surface_area);
            }
            tmin = rec.t + 1e-4;
        }
        return pdf;
    }

    virtual vec3 random(const vec3 &o) const {
        // Build
        // pick random face weighted by face area
        float r = drand48() * surface_area;
        int index = binarySearchIndices(r, 0, face_list.size() - 1);
        return face_list[index]->random(o);
    }

    int binarySearchIndices(float value, int start, int end) const {
        if (end < start)
            return start;
        int mid = start + (end - start) / 2;
        if (cumulativeAreas[mid] > value) {
            return binarySearchIndices(value, start, mid - 1);
        } else if (cumulativeAreas[mid] < value) {
            return binarySearchIndices(value, mid + 1, end);
        } else {
            return mid;
        }
    }

    vector<shared_ptr<Shape>> face_list;
    vector<float> cumulativeAreas;
    shared_ptr<Shape> bvh;
};


#endif //PATH_TRACING_MESH_H
