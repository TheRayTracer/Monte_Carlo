/* File: camera.h; Mode: C++; Tab-width: 3; Author: Simon Flannery;           */

#ifndef CAMERA_H
#define CAMERA_H

#include <stdlib.h>
#include <float.h>
#include "math.h"
#include "ray.h"

class Camera
{
public:
   Camera(const point3f& c, const vector3f& d, const vector3f& t, float s) : center(c), direction(d), up(t), size(s)
   {
      direction.Normalize();

      vector3f u = direction * vector3f::Dot(direction, up);
      up = up - u;

      up.Normalize();

      horizontal = vector3f::Cross(direction, up);
      horizontal.Normalize();

   // up = vector3f::Cross(horizontal, direction);
   // up.Normalize();
   }

   virtual Ray GenerateRay(const point2f& point) const = 0;
   virtual bool UseSamples() const = 0;

   static vector3f LookAt(const point3f& eye, const point3f& look_at)
   {
      vector3f d = look_at - eye;

      return d.Normalize();
   }

   virtual ~Camera() { }

protected:
   point3f center;
   vector3f direction;
   vector3f up;
   vector3f horizontal;
   float size;
};

class OrthographicCamera : public Camera
{
public:
   OrthographicCamera(const point3f& c, const vector3f& d, const vector3f& t, float s) : Camera(c, d, t, s)
   {

   }

   virtual Ray GenerateRay(const point2f& point) const
   {
      point3f origin = center + (point[x] - 0.5f) * size * horizontal + (point[y] - 0.5f) * size * up;

      Ray ray(origin, direction);

      return ray;
   }

   virtual bool UseSamples() const
   {
      return false;
   }

protected:
private:
};

class PerspectiveCamera : public Camera
{
public:
   PerspectiveCamera(const point3f& c, const vector3f& d, const vector3f& t, float a) : Camera(c, d, t, 0.0f), angle(a), focal_depth(1.0)
   {
      size = focal_depth * (float) tan(angle);
   }

   PerspectiveCamera(const point3f& c, const vector3f& d, const vector3f& t, float a, float fd) : Camera(c, d, t, 0.0f), angle(a), focal_depth(fd)
   {
      size = focal_depth * (float) tan(angle);
   }

   virtual Ray GenerateRay(const point2f& point) const
   {
      vector3f screen = center + direction + (point[x] - 0.5f) * size * horizontal + (point[y] - 0.5f) * size * up;

   /* Direction vectors are then calculated by subtracting the camera
      center point from the screen point. Don't forget to normalize. */
      vector3f d = screen - center;

      Ray ray(center, d.Normalize());

      return ray;
   }

   virtual bool UseSamples() const
   {
      return false;
   }

protected:
   float angle;
   float focal_depth;

private:
};

#endif
