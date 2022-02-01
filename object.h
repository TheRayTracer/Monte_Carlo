/* File: object.h; Mode: C++; Tab-width: 3; Author: Simon Flannery;           */

#ifndef OBJECT_H
#define OBJECT_H

#include <float.h>
#include "math.h"

class Ray;
class Hit;
class Material;

class Object
{
public:
   Object() : material(NULL) {    }

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const = 0;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const = 0;

   virtual ~Object() { }

protected:
   Material* material;

private:
};

class Solid : public Object
{
public:
   virtual bool Intersect(const Ray& ray, Hit& h1, Hit& h2, float tmin) const = 0;
};

class Sphere : public Solid
{
public:
   Sphere(const point3f& p, float r, Material* m);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool Intersect(const Ray& ray, Hit& h1, Hit& h2, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

protected:
   point3f point;
   float radius;

private:
};

class MotionSphere : public Sphere
{
public:
   MotionSphere(const point3f& p, float r, const vector3f& v, Material* m);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;

protected:
private:
   vector3f velocity;
};

class Plane : public Object
{
public:
   Plane(const vector3f& n, float offset, Material* m);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

protected:
private:
   float d;
   vector3f normal;
};

class Triangle : public Object
{
public:
   Triangle(const point3f& a, const point3f& b, const point3f& c, Material* m);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

protected:
private:
   point3f va, vb, vc;
   vector3f normal;
};

class Cone : public Object
{
public:
   Cone(const point3f& tip, const vector3f& ax, const float cos2a, const float h, Material* m);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

protected:
private:
   point3f v;
   vector3f axis;
   float cos2_angle_sq, height;
};

class XYRectangle : public Object
{
public:
   XYRectangle(const point2f low, const point2f up, const float _k, const float n, Material* m);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

protected:
private:
   float k; // k = z in (x, y, z)
   point2f lower, upper;
   vector3f normal;
};

class XZRectangle : public Object
{
public:
   XZRectangle(const point2f low, const point2f up, const float _k, const float n, Material* m);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

protected:
private:
   float k; // k = y in (x, y, z)
   point2f lower, upper;
   vector3f normal;
};

class YZRectangle : public Object
{
public:
   YZRectangle(const point2f low, const point2f up, const float _k, const float n, Material* m);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

protected:
private:
   float k; // k = x in (x, y, z)
   point2f lower, upper;
   vector3f normal;
};

class Cube : public Solid
{
public:
   Cube(const point3f& p, float size, Material* m);
   Cube(const point3f& p, const point3f& z, Material* m);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool Intersect(const Ray& ray, Hit& h1, Hit& h2, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

protected:
private:
   point3f max, min;
};

class Group : public Object
{
public:
   Group(size_t s);
   ~Group();

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

   void SetAt(size_t i, Object* obj);
   size_t GetSize() {   return size;   }

   void SetBB(const point3f& vmin, const point3f& vmax);

protected:
private:
   bool PossibleHit(const Ray& ray, float tmin) const;

   size_t size;
   Object** object;

   point3f bb_vmin, bb_vmax;
};

class CSGPair : public Object
{
public:
   enum class Type { Union, Intersection, Difference };

   CSGPair(Solid* sa, Solid* sb);
   ~CSGPair();

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

   void SetType(Type t) { type = t;   return; };

protected:
private:
   Type type;

   Solid* a, *b;
};

class Transform : public Object
{
public:
   Transform(const Matrix& m, Object* o);

   virtual bool Intersect(const Ray& ray, Hit& h, float tmin) const;
   virtual bool Intersect(const Ray& ray, Hit& h1, Hit& h2, float tmin) const;
   virtual bool ShadowIntersect(const Ray& ray, Hit& h, float tmin) const;

protected:
private:
   Matrix matrix;
   Object* object;
};

#endif
