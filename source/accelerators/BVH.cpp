//
// Created by hao on 3/1/19.
//

#include "BVH.h"

BVHNode::BVHNode(vector<shared_ptr<Shape> >::iterator l, int n, float time0, float time1) {
    auto axis = int(3 * drand48());
    if (axis == 0)
        sort(l, l + n, box_x_compare);
    else if (axis == 1)
        sort(l, l + n, box_y_compare);
    else
        sort(l, l + n, box_z_compare);

    if (n == 1) {
        left = right = l[0];
    } else if (n == 2) {
        left = l[0];
        right = l[1];
    } else {
        left = make_shared<BVHNode>(l, n / 2, time0, time1);
        right = make_shared<BVHNode>(l + n / 2, n - n / 2, time0, time1);
    }
    AABB box_left, box_right;
    if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    box = surrounding_box(box_left, box_right);
}

bool BVHNode::bounding_box(float t0, float t1, AABB &b) const {
    b = box;
    return true;
}

bool BVHNode::Intersect(const Ray &ray, float t_min, float t_max, Interaction &isect) const {
    if (box.IntersectP(ray, t_min, t_max)) {
        Interaction rec;
        if (left->Intersect(ray, t_min, t_max, rec)) {
            right->Intersect(ray, t_min, rec.t, rec);
            isect = rec;
            return true;
        } else {
            if (right->Intersect(ray, t_min, t_max, rec)) {
                isect = rec;
                return true;
            }
        }
    }
    return false;
}

bool box_x_compare(const shared_ptr<Shape> &a, const shared_ptr<Shape> &b) {
    AABB box_left, box_right;
    if (!a->bounding_box(0, 0, box_left) || !b->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().x < box_right.min().x)
        return true;
    else
        return false;
}

bool box_y_compare(const shared_ptr<Shape> &a, const shared_ptr<Shape> &b) {
    AABB box_left, box_right;
    if (!a->bounding_box(0, 0, box_left) || !b->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().y < box_right.min().y)
        return true;
    else
        return false;
}

bool box_z_compare(const shared_ptr<Shape> &a, const shared_ptr<Shape> &b) {
    AABB box_left, box_right;
    if (!a->bounding_box(0, 0, box_left) || !b->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().z < box_right.min().z)
        return true;
    else
        return false;
}
