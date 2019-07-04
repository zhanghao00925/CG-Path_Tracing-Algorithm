//
// Created by hao on 3/1/19.
//

#ifndef PATH_TRACING_SHAPELIST_H
#define PATH_TRACING_SHAPELIST_H


#include "../core/core.h"
#include "../accelerators/BVH.h"

#define __SHAPELIST_TYPE_1__

class ShapeList : public Shape {
public:
    ShapeList() {}

    ShapeList(vector<shared_ptr<Shape>> l) {
        bvh = make_shared<BVHNode>(l.begin(), l.size(), 0.0, 1.0);
	list = l;
    }

    virtual bool Intersect(const Ray &r, float t_min, float t_max, Interaction &rec) const {
        Interaction isec;
        if (bvh->Intersect(r, t_min, t_max, isec)) {
            rec = isec;
            return true;
        }
        return false;
    }

    virtual bool bounding_box(float t0, float t1, AABB &box) const {
        return bvh->bounding_box(t0, t1, box);
    }
#ifdef __SHAPELIST_TYPE_1__
    virtual float pdf_value(const vec3 &o, const vec3 &v) const {
        float weight = 1.0 / list.size();
        float sum = 0;
        for (int i = 0; i < list.size(); i++) {
            sum += weight * list[i]->pdf_value(o, v);
        }
        return sum;
    }

    virtual vec3 random(const vec3 &o) const {
        int index = int(drand48() * list.size());
        return list[index]->random(o);
    }
#else
   virtual float pdf_value(const vec3 &o, const vec3 &v) const {
       float weight_sum = 0;
       vector<float> weight_list(list_size);
       AABB box;
       for (int i = 0; i < list_size; i++) {
           list[i]->bounding_box(0, 1, box);
           weight_sum += 1 / box.short_distance(o);
           weight_list[i] = (1 / box.short_distance(o));
       }
       float pdf_sum = 0;
       for (int i = 0; i < list_size; i++) {
           pdf_sum += weight_list[i] / weight_sum * list[i]->pdf_value(o, v);
       }
       return pdf_sum;
   }

   virtual vec3 random(const vec3 &o) const {
       float weight_sum = 0;
       AABB box;
       vector<float> cumulative_weight(list_size);
       for (int i = 0; i < list_size; i++) {
           list[i]->bounding_box(0, 1, box);
           weight_sum += 1 / box.short_distance(o);
           cumulative_weight[i] = weight_sum;
       }
       float r = drand48() * weight_sum;
       int index = binarySearchIndices(cumulative_weight, r, 0, list_size - 1);
       return list[index]->random(o);
   }
#endif
    int binarySearchIndices(const vector<float> &weight_list, float value, int start, int end) const {
        if (end < start)
            return start;
        int mid = start + (end - start) / 2;
        if (weight_list[mid] > value) {
            return binarySearchIndices(weight_list, value, start, mid - 1);
        } else if (weight_list[mid] < value) {
            return binarySearchIndices(weight_list, value, mid + 1, end);
        } else {
            return mid;
        }
    }

    vector<shared_ptr<Shape>> list;
    shared_ptr<Shape> bvh;
    int list_size;
};


#endif //PATH_TRACING_SHAPELIST_H
