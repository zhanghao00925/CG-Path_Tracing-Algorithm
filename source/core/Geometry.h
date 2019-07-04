//
// Created by hao on 2/28/19.
//

#ifndef PATH_TRACING_GEOMETRY_H
#define PATH_TRACING_GEOMETRY_H

#include "Ray.h"

class AABB
{
  public:
    AABB() = default;

    AABB(const vec3 &_pMin, const vec3 &_pMax) : pMin(_pMin), pMax(_pMax)
    {
        float epsilon = 1e-5;
        if (pMax.x - pMin.x < epsilon)
        {
            pMin.x -= epsilon;
            pMax.x += epsilon;
        }
        if (pMax.y - pMin.y < epsilon)
        {
            pMin.y -= epsilon;
            pMax.y += epsilon;
        }
        if (pMax.z - pMin.z < epsilon)
        {
            pMin.z -= epsilon;
            pMax.z += epsilon;
        }
    }

    vec3 min() const { return pMin; }

    vec3 max() const { return pMax; }

    float short_distance(const vec3 &o) const
    {
        float x, y, z;
        x = abs(pMax.x - o.x) < abs(pMin.x - o.x) ? pMax.x : pMin.x;
        y = abs(pMax.y - o.y) < abs(pMin.y - o.y) ? pMax.y : pMin.y;
        z = abs(pMax.z - o.z) < abs(pMin.z - o.z) ? pMax.z : pMin.z;
        return length(vec3(x, y, z) - o);
    }

    bool IntersectP(const Ray &ray, float tmin, float tmax) const
    {
        for (int a = 0; a < 3; a++)
        {
            float t0 = std::min((pMin[a] - ray.origin()[a]) / ray.direction()[a],
                                (pMax[a] - ray.origin()[a]) / ray.direction()[a]);
            float t1 = std::max((pMin[a] - ray.origin()[a]) / ray.direction()[a],
                                (pMax[a] - ray.origin()[a]) / ray.direction()[a]);
            tmin = std::max(t0, tmin);
            tmax = std::min(t1, tmax);
            if (tmax <= tmin)
                return false;
        }
        return true;
    }

    vec3 pMin, pMax;
};

AABB surrounding_box(AABB box0, AABB box1);

#endif //PATH_TRACING_GEOMETRY_H
