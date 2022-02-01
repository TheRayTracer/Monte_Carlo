/* File: object.cpp; Mode: C++; Tab-width: 3; Author: Simon Flannery;         */

#include <stdlib.h>
#include "object.h"
#include "ray.h"
#include "hit.h"

Sphere::Sphere(const point3f& p, float r, Material* m) : point(p), radius(r) {   material = m;   }

bool Sphere::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   vector3f o(ray.GetOrigin() - point);

// float a = vector3f::Dot(ray.GetDirection(), ray.GetDirection()); /* Always 1! */
   float b = vector3f::Dot(o, ray.GetDirection());
   float c = vector3f::Dot(o, o) - radius * radius;
   float d = b * b - c; /* The Discriminant. */

   if (d > 0.0f) /* Two Solutions. */
   {
      d = (float) sqrt(d);

      float t1 = (-b - d);
      float t2 = (-b + d);

      if (t1 > tmin && t1 < h.GetT())
      {
         vector3f n = ray.PointAtParameter(t1) - point;
         
      // h.intersection_point = ray.GetOrigin() + t1 * ray.GetDirection() - point;
         h.Set(t1, material, n.Normalize(), ray);
         result = true;
      }
      else if (t2 > tmin && t2 < h.GetT())
      {
         vector3f n = ray.PointAtParameter(t2) - point;

      // h.intersection_point = ray.GetOrigin() + t2 * ray.GetDirection() - point;
         h.Set(t2, material, n.Normalize(), ray);
         result = true;
      }
   }

   return result;
}

bool Sphere::Intersect(const Ray& ray, Hit& h1, Hit& h2, float tmin) const
{
   bool result = false;

   vector3f o(ray.GetOrigin() - point);

// float a = vector3f::Dot(ray.GetDirection(), ray.GetDirection()); /* Always 1! */
   float b = vector3f::Dot(o, ray.GetDirection());
   float c = vector3f::Dot(o, o) - radius * radius;
   float d = b * b - c; /* The Discriminant. */

   if (d > 0.0f) /* Two Solutions. */
   {
      d = (float) sqrt(d);

      float t1 = (-b - d);
      float t2 = (-b + d);

      if (t1 > tmin && t1 < h1.GetT())
      {
         vector3f n1 = ray.PointAtParameter(t1) - point;
         vector3f n2 = ray.PointAtParameter(t2) - point;

         h1.Set(t1, material, n1.Normalize(), ray);
         h2.Set(t2, material, n2.Normalize(), ray);

         result = true;
      }
      else if (t2 > tmin && t2 < h1.GetT())
      {
         vector3f n1 = ray.PointAtParameter(t1) - point;
         vector3f n2 = ray.PointAtParameter(t2) - point;

         h1.Set(t2, material, n2.Normalize(), ray);
         h2.Set(t1, material, n1.Normalize(), ray);

         result = true;
      }
   }

   return result;
}

bool Sphere::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}

MotionSphere::MotionSphere(const point3f& p, float r, const vector3f& v, Material* m) : Sphere(p, r, m), velocity(v) {   }

bool MotionSphere::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   float t = rand() / (float) RAND_MAX;

   vector3f p = point + velocity * t;

   vector3f o(ray.GetOrigin() - p);

// float a = vector3f::Dot(ray.GetDirection(), ray.GetDirection()); /* Always 1! */
   float b = vector3f::Dot(ray.GetDirection(), o);
   float c = vector3f::Dot(o, o) - radius * radius;
   float d = b * b - c; /* The Discriminant. */

   if (d > 0.0f) /* Two Solutions. */
   {
      d = (float) sqrt(d);

      float t1 = (-b - d);
      float t2 = (-b + d);

      if (t1 > tmin && t1 < h.GetT())
      {
         vector3f n = ray.PointAtParameter(t1) - p;
         
      // h.intersection_point = ray.GetOrigin() + t1 * ray.GetDirection() - point;
         h.Set(t1, material, n.Normalize(), ray);
         result = true;
      }
      else if (t2 > tmin && t2 < h.GetT())
      {
         vector3f n = ray.PointAtParameter(t2) - p;

      // h.intersection_point = ray.GetOrigin() + t2 * ray.GetDirection() - point;
         h.Set(t2, material, n.Normalize(), ray);
         result = true;
      }
   }

   return result;
}

Plane::Plane(const vector3f& n, float offset, Material* m) : d(-offset), normal(n) {   material = m;   normal.Normalize();   }

bool Plane::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   float denom = vector3f::Dot(normal, ray.GetDirection());

   if (denom != 0.0f)
   {
      float t = -(d + vector3f::Dot(normal, ray.GetOrigin())) / denom;

      if (t > tmin && t < h.GetT())
      {
         h.Set(t, material, normal, ray);
         result = true;
      }
   }

   return result;
}

bool Plane::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}

Triangle::Triangle(const point3f& a, const point3f& b, const point3f& c, Material* m) : va(a), vb(b), vc(c)
{
   material = m;

   vector3f ea = va - vc;
   vector3f eb = vb - vc;

   normal = vector3f::Cross(ea, eb);
   normal.Normalize();
}

bool Triangle::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   float a = Det3x3(va[x] - vb[x], va[x] - vc[x], ray.GetDirection()[x],
                    va[y] - vb[y], va[y] - vc[y], ray.GetDirection()[y],
                    va[z] - vb[z], va[z] - vc[z], ray.GetDirection()[z]);
   float t = Det3x3(va[x] - vb[x], va[x] - vc[x], va[x] - ray.GetOrigin()[x],
                    va[y] - vb[y], va[y] - vc[y], va[y] - ray.GetOrigin()[y],
                    va[z] - vb[z], va[z] - vc[z], va[z] - ray.GetOrigin()[z]);
         t = t / a;

   if (t > tmin && t < h.GetT())
   {
      float g = Det3x3(va[x] - vb[x], va[x] - ray.GetOrigin()[x], ray.GetDirection()[x],
                       va[y] - vb[y], va[y] - ray.GetOrigin()[y], ray.GetDirection()[y],
                       va[z] - vb[z], va[z] - ray.GetOrigin()[z], ray.GetDirection()[z]);
            g = g / a;

      if (g >= 0.0f && g <= 1.0f)
      {
         float b = Det3x3(va[x] - ray.GetOrigin()[x], va[x] - vc[x], ray.GetDirection()[x],
                          va[y] - ray.GetOrigin()[y], va[y] - vc[y], ray.GetDirection()[y],
                          va[z] - ray.GetOrigin()[z], va[z] - vc[z], ray.GetDirection()[z]);
               b = b / a;

         if (b >= 0.0f && b <= (1.0f - g))
         {
            result = true;
            h.Set(t, material, normal, ray);
         }
      }
   }

   return result;
}

bool Triangle::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}

Cone::Cone(const point3f& tip, const vector3f& ax, const float cos2a, const float h, Material* m) : v(tip), axis(ax), cos2_angle_sq(cos2a), height(h)
{
   material = m;

   axis.Normalize();
}

bool Cone::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   vector3f co = ray.GetOrigin() - v;

   float a = vector3f::Dot(ray.GetDirection(), axis) * vector3f::Dot(ray.GetDirection(), axis) - cos2_angle_sq;
   float b = 2.0f * (vector3f::Dot(ray.GetDirection(), axis) * vector3f::Dot(co, axis) - vector3f::Dot(ray.GetDirection(), co) * cos2_angle_sq);
   float c = vector3f::Dot(co, axis) * vector3f::Dot(co, axis) - vector3f::Dot(co, co) * cos2_angle_sq;
   float d = b * b - 4.0f * a * c;

   if (d > 0.0f) /* Two possible solutions. */
   {
      d = (float) sqrt(d);
      float t1 = (-b - d) / (2.0f * a);
      float t2 = (-b + d) / (2.0f * a);

      float t = t1;
      if (t2 < t1) t = t2;

      vector3f cp = ray.GetOrigin() + t * ray.GetDirection() - v;
      float q = vector3f::Dot(cp, axis);

      if (q < 0.0f || q > height)
      {
         result = false;
      }
      else
      {
         vector3f n = (cp * vector3f::Dot(axis, cp) / vector3f::Dot(cp, cp)) - axis;
         n.Normalize();

         if (t > tmin && t < h.GetT())
         {
            h.Set(t, material, n, ray);
            result = true;
         }
      }
   }

   return result;
}

bool Cone::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}

XYRectangle::XYRectangle(const point2f low, const point2f up, const float _k, const float n, Material* m)
{
   material = m;

   lower = low;
   upper = up;

   k = _k;

   normal = vector3f(0.0f, 0.0f, n);
   normal.Normalize();
}

bool XYRectangle::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   float t = (k - ray.GetOrigin()[z]) * ray.GetInverseDirectionForAABoxFaceIntersection()[z];

   if (t > tmin && t < h.GetT())
   {
      float fx = ray.GetOrigin()[x] + t * ray.GetDirection()[x];
      float fy = ray.GetOrigin()[y] + t * ray.GetDirection()[y];

      if (fx > lower[x] && fx < upper[x] && fy > lower[y] && fy < upper[y])
      {
         h.Set(t, material, normal, ray);

         result = true;
      }
   }

   return result;
}

bool XYRectangle::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}

XZRectangle::XZRectangle(const point2f low, const point2f up, const float _k, const float n, Material* m)
{
   material = m;

   lower = low;
   upper = up;

   k = _k;

   normal = vector3f(0.0f, n, 0.0f);
   normal.Normalize();
}

bool XZRectangle::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   float t = (k - ray.GetOrigin()[y]) * ray.GetInverseDirectionForAABoxFaceIntersection()[y];

   if (t > tmin && t < h.GetT())
   {
      float fx = ray.GetOrigin()[x] + t * ray.GetDirection()[x];
      float fz = ray.GetOrigin()[z] + t * ray.GetDirection()[z];

      if (fx > lower[x] && fx < upper[x] && fz > lower[y] && fz < upper[y])
      {
         h.Set(t, material, normal, ray);

         result = true;
      }
   }

   return result;
}

bool XZRectangle::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}

YZRectangle::YZRectangle(const point2f low, const point2f up, const float _k, const float n, Material* m)
{
   material = m;

   lower = low;
   upper = up;

   k = _k;

   normal = vector3f(n, 0.0f, 0.0f);
   normal.Normalize();
}

bool YZRectangle::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   float t = (k - ray.GetOrigin()[x]) * ray.GetInverseDirectionForAABoxFaceIntersection()[x];

   if (t > tmin && t < h.GetT())
   {
      float fy = ray.GetOrigin()[y] + t * ray.GetDirection()[y];
      float fz = ray.GetOrigin()[z] + t * ray.GetDirection()[z];

      if (fy > lower[x] && fy < upper[x] && fz > lower[y] && fz < upper[y])
      {
         h.Set(t, material, normal, ray);

         result = true;
      }
   }

   return result;
}

bool YZRectangle::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}

Cube::Cube(const point3f& p, float size, Material* m)
{
   size = size / 2.0f;

   min = p - point3f(size, size, size);
   max = p + point3f(size, size, size);

   material = m;
}

Cube::Cube(const point3f& p, const point3f& z, Material* m)
{
   min = p;
   max = z;

   material = m;
}

bool Cube::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   vector3f n;
   float tmax = FLT_MAX;

   float tx0 = (min[x] - ray.GetOrigin()[x]) * ray.GetInverseDirectionForAABoxFaceIntersection()[x];
   float tx1 = (max[x] - ray.GetOrigin()[x]) * ray.GetInverseDirectionForAABoxFaceIntersection()[x];

   if (tx0 < tx1)
   {
      if (tx0 > tmin)
      {
         tmin = tx0;
         n = vector3f(-1.0f, 0.0f, 0.0f);
      }

      if (tx1 < tmax)
      {
         tmax = tx1;
      }
   }
   else
   {
      if (tx1 > tmin)
      {
         tmin = tx1;
         n = vector3f(1.0f, 0.0f, 0.0f);
      }

      if (tx0 < tmax)
      {
         tmax = tx0;
      }
   }

   if (tmin < tmax)
   {
      float ty0 = (min[y] - ray.GetOrigin()[y]) * ray.GetInverseDirectionForAABoxFaceIntersection()[y];
      float ty1 = (max[y] - ray.GetOrigin()[y]) * ray.GetInverseDirectionForAABoxFaceIntersection()[y];

      if (ty0 < ty1)
      {
         if (ty0 > tmin)
         {
            tmin = ty0;
            n = vector3f(0.0f, -1.0f, 0.0f);
         }

         if (ty1 < tmax)
         {
            tmax = ty1;
         }
      }
      else
      {
         if (ty1 > tmin)
         {
            tmin = ty1;
            n = vector3f(0.0f, 1.0f, 0.0f);
        }

         if (ty0 < tmax)
         {
            tmax = ty0;
         }
      }

      if (tmin < tmax)
      {
         float tz0 = (min[z] - ray.GetOrigin()[z]) * ray.GetInverseDirectionForAABoxFaceIntersection()[z];
         float tz1 = (max[z] - ray.GetOrigin()[z]) * ray.GetInverseDirectionForAABoxFaceIntersection()[z];

         if (tz0 < tz1)
         {
            if (tz0 > tmin)
            {
               tmin = tz0;
               n = vector3f(0.0f, 0.0f, -1.0f);
            }

            if (tz1 < tmax)
            {
               tmax = tz1;
            }
         }
         else
         {
            if (tz1 > tmin)
            {
               tmin = tz1;
               n = vector3f(0.0f, 0.0f, 1.0f);
            }

            if (tz0 < tmax)
            {
               tmax = tz0;
            }
         }

         if (tmin < tmax)
         {
            if (tmin > 0.0 && tmin < h.GetT())
            {
               h.Set(tmin, material, n, ray);

               result = true;
            }
         }
      }
   }

   return result;
}

bool Cube::Intersect(const Ray& ray, Hit& h1, Hit& h2, float tmin) const
{
   bool result = false;

   vector3f nmin, nmax;
   float tmax = FLT_MAX;

   float tx0 = (min[x] - ray.GetOrigin()[x]) * ray.GetInverseDirectionForAABoxFaceIntersection()[x];
   float tx1 = (max[x] - ray.GetOrigin()[x]) * ray.GetInverseDirectionForAABoxFaceIntersection()[x];

   if (tx0 < tx1)
   {
      if (tx0 > tmin)
      {
         tmin = tx0;
         nmin = vector3f(-1.0f, 0.0f, 0.0f);
      }

      if (tx1 < tmax)
      {
         tmax = tx1;
         nmax = vector3f(1.0f, 0.0f, 0.0f);
      }
   }
   else
   {
      if (tx1 > tmin)
      {
         tmin = tx1;
         nmin = vector3f(1.0f, 0.0f, 0.0f);
      }

      if (tx0 < tmax)
      {
         tmax = tx0;
         nmax = vector3f(-1.0f, 0.0f, 0.0f);
      }
   }

   if (tmin < tmax)
   {
      float ty0 = (min[y] - ray.GetOrigin()[y]) * ray.GetInverseDirectionForAABoxFaceIntersection()[y];
      float ty1 = (max[y] - ray.GetOrigin()[y]) * ray.GetInverseDirectionForAABoxFaceIntersection()[y];

      if (ty0 < ty1)
      {
         if (ty0 > tmin)
         {
            tmin = ty0;
            nmin = vector3f(0.0f, -1.0f, 0.0f);
         }

         if (ty1 < tmax)
         {
            tmax = ty1;
            nmax = vector3f(0.0f, 1.0f, 0.0f);
         }
      }
      else
      {
         if (ty1 > tmin)
         {
            tmin = ty1;
            nmin = vector3f(0.0f, 1.0f, 0.0f);
         }

         if (ty0 < tmax)
         {
            tmax = ty0;
            nmax = vector3f(0.0f, -1.0f, 0.0f);
         }
      }

      if (tmin < tmax)
      {
         float tz0 = (min[z] - ray.GetOrigin()[z]) * ray.GetInverseDirectionForAABoxFaceIntersection()[z];
         float tz1 = (max[z] - ray.GetOrigin()[z]) * ray.GetInverseDirectionForAABoxFaceIntersection()[z];

         if (tz0 < tz1)
         {
            if (tz0 > tmin)
            {
               tmin = tz0;
               nmin = vector3f(0.0f, 0.0f, -1.0f);
            }

            if (tz1 < tmax)
            {
               tmax = tz1;
               nmax = vector3f(0.0f, 0.0f, 1.0f);
            }
         }
         else
         {
            if (tz1 > tmin)
            {
               tmin = tz1;
               nmin = vector3f(0.0f, 0.0f, 1.0f);
            }

            if (tz0 < tmax)
            {
               tmax = tz0;
               nmax = vector3f(0.0f, 0.0f, -1.0f);
            }
         }

         if (tmin < tmax)
         {
            if (tmin > 0.0 && tmin < h1.GetT())
            {
               h1.Set(tmin, material, nmin, ray);
               h2.Set(tmax, material, nmax, ray);

               result = true;
            }
         }
      }
   }

   return result;
}

bool Cube::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}

Group::Group(size_t s) : size(s), bb_vmin(FLT_MAX, FLT_MAX, FLT_MAX), bb_vmax(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
   object = new Object*[size];

   for (size_t i = 0; i < size; ++i)
   {
      object[i] = NULL;
   }
}

Group::~Group()
{
   for (size_t i = 0; i < size; ++i)
   {
      delete object[i];
   }

   delete [] object;
}

bool Group::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   if (PossibleHit(ray, tmin) != false)
   {
      for (size_t i = 0; i < size; ++i)
      {
         if (object[i]->Intersect(ray, h, tmin) != false)
         {
            result = true;
         }
      }
   }

   return result;
}

bool Group::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   for (size_t i = 0; i < size && result == false; ++i)
   {
      result = object[i]->Intersect(ray, h, tmin);
   }

   return result;
}

void Group::SetAt(size_t i, Object* obj)
{
   if (i < size)
   {
      object[i] = obj;
   }

   return;
}

void Group::SetBB(const point3f& vmin, const point3f& vmax)
{
   bb_vmin = vmin;
   bb_vmax = vmax;

   return;
}

bool Group::PossibleHit(const Ray& ray, float tmin) const
{
   float tmax = FLT_MAX;
   bool result = true; /* Assume a hit. */

   if (bb_vmax > bb_vmin)
   {
      float t0 = (bb_vmin[x] - ray.GetOrigin()[x]) * ray.GetInverseDirectionForAABoxFaceIntersection()[x];
      float t1 = (bb_vmax[x] - ray.GetOrigin()[x]) * ray.GetInverseDirectionForAABoxFaceIntersection()[x];

      if (ray.GetInverseDirectionForAABoxFaceIntersection()[x] < 0.0f)
      {
         float tmp = t0;
         t0 = t1;
         t1 = tmp;
      }

      tmin = t0 > tmin ? t0 : tmin;
      tmax = t1 < tmax ? t1 : tmax;

      if (tmin > tmax)
      {
         result = false;
      }
      else
      {
         t0 = (bb_vmin[y] - ray.GetOrigin()[y]) * ray.GetInverseDirectionForAABoxFaceIntersection()[y];
         t1 = (bb_vmax[y] - ray.GetOrigin()[y]) * ray.GetInverseDirectionForAABoxFaceIntersection()[y];

         if (ray.GetInverseDirectionForAABoxFaceIntersection()[y] < 0.0f)
         {
            float tmp = t0;
            t0 = t1;
            t1 = tmp;
         }

         tmin = t0 > tmin ? t0 : tmin;
         tmax = t1 < tmax ? t1 : tmax;

         if (tmin > tmax)
         {
            result = false;
         }
         else
         {
            t0 = (bb_vmin[z] - ray.GetOrigin()[z]) * ray.GetInverseDirectionForAABoxFaceIntersection()[z];
            t1 = (bb_vmax[z] - ray.GetOrigin()[z]) * ray.GetInverseDirectionForAABoxFaceIntersection()[z];

            if (ray.GetInverseDirectionForAABoxFaceIntersection()[z] < 0.0f)
            {
               float tmp = t0;
               t0 = t1;
               t1 = tmp;
            }

            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;

            if (tmin > tmax)
            {
               result = false;
            }
         }
      }
   }

   return result;
}

CSGPair::CSGPair(Solid* sa, Solid* sb) : a(sa), b(sb), type(Type::Union)
{
}

CSGPair::~CSGPair()
{
   delete a;
   delete b;
}

bool CSGPair::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   bool result = false;

   if (type == Type::Difference)
   {
      Hit h1_min, h1_max, h2_min, h2_max;

      result = a->Intersect(ray, h1_min, h1_max, tmin);

      if (result != false)
      {
         bool h2 = b->Intersect(ray, h2_min, h2_max, tmin);

         if (h2 != false)
         {
            if (h1_min.GetT() < h2_min.GetT())
            {
               h.Set(h1_min.GetT(), h1_min.GetMaterial(), h1_min.GetNormal(), ray);
            }
            else if (h2_max.GetT() < h1_max.GetT())
            {
               if (h2_max.GetT() > h1_min.GetT())
               {
                  h.Set(h2_max.GetT(), h2_max.GetMaterial(), h2_max.GetNormal().Negate(), ray);
               }
               else
               {
                  h.Set(h1_min.GetT(), h1_min.GetMaterial(), h1_min.GetNormal(), ray);
               }
            }
            else
            {
               result = false;
            }
         }
         else
         {
            h.Set(h1_min.GetT(), h1_min.GetMaterial(), h1_min.GetNormal(), ray);
         }
      }
   }
   else if (type == Type::Intersection)
   {
      Hit h1_min, h1_max, h2_min, h2_max;

      result = a->Intersect(ray, h1_min, h1_max, tmin);

      if (result != false)
      {
         bool h2 = b->Intersect(ray, h2_min, h2_max, tmin);

         if (h2 != false)
         {
            if ((h2_min.GetT() < h1_min.GetT()) && (h2_max.GetT() > h1_min.GetT()))
            {
               h.Set(h1_min.GetT(), h1_min.GetMaterial(), h1_min.GetNormal(), ray);
            }
            else if ((h1_min.GetT() < h2_min.GetT()) && (h1_max.GetT() > h2_min.GetT()))
            {
               if (h2_max.GetT() < h.GetT())
               {
                  h.Set(h2_min.GetT(), h2_min.GetMaterial(), h2_min.GetNormal(), ray);
               }
            }
            else
            {
               result = false;
            }
         }
      }
   }
   else if (type == Type::Union)
   {
      Hit h1_max;
      if (a->Intersect(ray, h, h1_max, tmin) != false)
      {
         result = true;
      }

      Hit h2_max;
      if (b->Intersect(ray, h, h2_max, tmin) != false)
      {
         result = true;
      }
   }

   return result;
}

bool CSGPair::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}

Transform::Transform(const Matrix& m, Object* o) : matrix(m), object(o) { }

bool Transform::Intersect(const Ray& ray, Hit& h, float tmin) const
{
   vector3f origin    = ray.GetOrigin();
   vector3f direction = ray.GetDirection();

/* Remember, points and directions transform differently! */
   matrix.Inverse().Transform(origin);
   matrix.Inverse().TransformDirection(direction);

   const float scale = direction.Length();

   const Ray new_ray(origin, direction.Normalize());

   h.Set(h.GetT() * scale, h.GetMaterial(), h.GetNormal(), new_ray);

   const bool intersect = object->Intersect(new_ray, h, tmin);

   if (intersect != false)
   {
      vector3f normal = h.GetNormal();
      
      matrix.Inverse().Transpose().TransformDirection(normal);
      h.Set(h.GetT() / scale, h.GetMaterial(), normal, ray);
   }
   else
   {
      h.Set(h.GetT() / scale, h.GetMaterial(), h.GetNormal(), ray);
   }

   return intersect;
}

bool Transform::Intersect(const Ray& ray, Hit& h1, Hit&, float tmin) const
{
   return Intersect(ray, h1, tmin);
}

bool Transform::ShadowIntersect(const Ray& ray, Hit& h, float tmin) const
{
   return Intersect(ray, h, tmin);
}
