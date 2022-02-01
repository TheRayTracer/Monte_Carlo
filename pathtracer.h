/* File: raytracer.h; Mode: C++; Tab-width: 3; Author: Simon Flannery;        */

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "math.h"
#include "hit.h"
#include "ray.h"

#define EPSILON  1e-4f

class Scene;

class PathTracer
{
public:
   PathTracer(Scene* s, size_t max_bounces) : scene(s), max_bounces(max_bounces), epsilon(EPSILON) {   }

   color3f TracePath(const Ray& ray, size_t bounce) const;

private:
   Scene* scene;
   size_t max_bounces;
   float epsilon;
};

#endif
