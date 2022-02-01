/* File: material.h; Mode: C++; Tab-width: 3; Author: Simon Flannery;         */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "math.h"
#include "ray.h"
#include "hit.h"
#include "perlin.h"

class Material
{
public:
   Material() { }

   Material(const vector3f& c)
   {
      color = c;
   }

   virtual color3f GetColor(const point3f&) const {   return color;   }

   virtual color3f Emitted(const point3f&) const { return color3f(0.0f, 0.0f, 0.0f); }

   virtual bool IsSpecular(const point3f&) const {   return false;   }

   virtual bool Scatter(const Ray& ray, const Hit& hit, vector3f& scattered) const = 0;

// virtual float ScatterPdf(const Hit& hit, const vector3f& scattered) const {   return 0.0f;   }

   virtual ~Material() { }

protected:
   color3f color;

private:
   friend class NoiseMaterial;
};

class DiffuseMaterial : public Material
{
public:
   DiffuseMaterial(const color3f& c, const color3f light = color3f(0.0f, 0.0f, 0.0f)) : Material(c), glow(light)
   {

   }

   virtual bool Scatter(const Ray& ray, const Hit& hit, vector3f& scattered) const
   {
      return true;
   }

// virtual float ScatterPdf(const Hit& hit, const vector3f& scattered) const
// {
//    float cosine = vector3f::Dot(hit.GetNormal(), scattered);
//
//    return (float) fmax(FLT_EPSILON, cosine / PI);
// }

   virtual color3f Emitted(const point3f&) const {   return glow;   }

protected:
private:
   color3f glow;
};

class ReflectiveMaterial : public Material
{
public:
   ReflectiveMaterial(const color3f& c, const float b = 0.0f) : Material(c), blur(b)
   {

   }

   virtual bool IsSpecular(const point3f&) const { return true; }

   virtual bool Scatter(const Ray& ray, const Hit& hit, vector3f& scattered) const
   {
      scattered = ReflectDirection(ray.GetDirection(), hit.GetNormal()) + blur * vector3f::RandomInHemisphere(hit.GetNormal());

      return true;
   }

protected:
   static vector3f ReflectDirection(const vector3f& d, const vector3f& n)
   {
      return d - (2.0f * vector3f::Dot(d, n) * n);
   }

private:
   float blur;
};

class GlassMaterial : public ReflectiveMaterial
{
public:
   GlassMaterial(const color3f& c, const float ir) : ReflectiveMaterial(c), refraction_index(ir)
   {

   }

   virtual bool IsSpecular(const point3f&) const { return true; }

   virtual bool Scatter(const Ray& ray, const Hit& hit, vector3f& scattered) const
   {
      float refraction_ratio = 1.0f / refraction_index;

      vector3f normal = hit.GetNormal();
      
      if (vector3f::Dot(ray.GetDirection(), normal) > 0.0f)
      {
         refraction_ratio = refraction_index;

         normal.Negate();
      }

      float cos_theta = (float) fmin(vector3f::Dot(-ray.GetDirection(), normal), 1.0f);

      float sin_theta = (float) sqrt(1.0f - (cos_theta * cos_theta));

      if ((refraction_ratio * sin_theta) > 1.0f ||
           Reflectance(cos_theta, refraction_ratio) > random_float())
      {
         scattered = ReflectDirection(ray.GetDirection(), normal);
      }
      else
      {
         scattered = RefractDirection(ray.GetDirection(), normal, cos_theta, refraction_ratio);
      }

      return true;
   }

protected:
private:
   static vector3f RefractDirection(const vector3f& uv, const vector3f& n, const float cos_theta, const float etai_over_etat)
   {
      vector3f perp = etai_over_etat * (uv + cos_theta * n);

      vector3f parallel = (float) -sqrt(fabs(1.0f - perp.LengthSq())) * n;

      return perp + parallel;
   }

   static double Reflectance(float cosine, float ref_idx)
   {
   // Schlick's approximation for reflectance.
      float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);

      r0 = r0 * r0;

      return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
   }

   float refraction_index;
};

class Checkerboard : public Material
{
public:
   Checkerboard(Matrix m, Material* m1, Material* m2) : matrix(m), material1(m1), material2(m2)
   {

   }

   virtual color3f GetColor(const point3f& p) const
   {
      point3f t = p;
      matrix.Transform(t);

      int cx = (int) floor(t[x]);
      int cy = (int) floor(t[y]);
      int cz = (int) floor(t[z]);

      color3f color;
      if ((cx + cy + cz) % 2 == 0) /* Even! */
      {
         color = material1->GetColor(p);
      }
      else
      {
         color = material2->GetColor(p);
      }

      return color;
   }

   virtual color3f Emitted(const point3f& p) const
   {
      point3f t = p;
      matrix.Transform(t);

      int cx = (int) floor(t[x]);
      int cy = (int) floor(t[y]);
      int cz = (int) floor(t[z]);

      color3f emitted;
      if ((cx + cy + cz) % 2 == 0) /* Even! */
      {
         emitted = material1->Emitted(p);
      }
      else
      {
         emitted = material2->Emitted(p);
      }

      return emitted;
   }

   virtual bool IsSpecular(const point3f& p) const
   {
      point3f t = p;
      matrix.Transform(t);

      int cx = (int) floor(t[x]);
      int cy = (int) floor(t[y]);
      int cz = (int) floor(t[z]);

      bool specular;
      if ((cx + cy + cz) % 2 == 0) /* Even! */
      {
         specular = material1->IsSpecular(p);
      }
      else
      {
         specular = material2->IsSpecular(p);
      }

      return specular;
   }

   virtual bool Scatter(const Ray& ray, const Hit& hit, vector3f& scattered) const
   {
      point3f t = hit.GetIntersectionPoint();
      matrix.Transform(t);

      int cx = (int) floor(t[x]);
      int cy = (int) floor(t[y]);
      int cz = (int) floor(t[z]);

      bool scatter;
      if ((cx + cy + cz) % 2 == 0) /* Even! */
      {
         scatter = material1->Scatter(ray, hit, scattered);
      }
      else
      {
         scatter = material2->Scatter(ray, hit, scattered);
      }

      return scatter;
   }


protected:
   Matrix matrix;
   Material* material1, * material2;

private:
};

class NoiseMaterial : public Material
{
public:
   NoiseMaterial(Matrix m, Material* m1, Material* m2, size_t oct) : matrix(m), material1(m1), material2(m2), octaves(oct)
   {

   }

   virtual bool Scatter(const Ray& ray, const Hit& hit, vector3f& scattered) const
   {
      scattered = hit.GetNormal() + vector3f::RandomInHemisphere(hit.GetNormal());

      return true;
   }

protected:
   virtual float CalulateNoise(const point3f& point) const
   {
      point3f t = point;
      matrix.Transform(t);

      float noise = 0.0f;

      for (size_t i = 0, j = 1; i < octaves; ++i, j = 2 * j)
      {
         noise = noise + (float) (Perlin::noise(t[x] * j, t[y] * j, t[z] * j) / j);
      }

      return noise;
   }

   Matrix matrix;
   Material* material1, * material2;
   size_t octaves;

private:
   color3f GetColor(const point3f& point) const
   {
      float noise = CalulateNoise(point);

      color3f color_range = material1->color - material2->color;

      color3f color = (color_range * noise) + material2->color;

      return color.Clamp();
   }
};

class MarbleMaterial : public NoiseMaterial
{
public:
   MarbleMaterial(Matrix m, Material* m1, Material* m2, size_t oct, float fre, float amp) : NoiseMaterial(m, m1, m2, oct), frequency(fre), amplitude(amp)
   {

   }

protected:
   virtual float CalulateNoise(const point3f& point) const
   {
      point3f t = point;
      matrix.Transform(t);

      float noise = 0.0f;

      for (size_t i = 0, j = 1; i < octaves; ++i, j = 2 * j)
      {
         noise = noise + (float) (Perlin::noise(t[x] * j, t[y] * j, t[z] * j) / j);
      }

      noise = (float) sin(frequency * t[x] + amplitude * noise);

      return noise;
   }

private:
   float frequency;
   float amplitude;
};

class WoodMaterial : public NoiseMaterial
{
public:
   WoodMaterial(Matrix m, Material* m1, Material* m2, size_t oct, float fre, float amp) : NoiseMaterial(m, m1, m2, oct), frequency(fre), amplitude(amp)
   {

   }

protected:
   virtual float CalulateNoise(const point3f& point) const
   {
      point3f t = point;
      matrix.Transform(t);

      float noise = 0.0f;

      for (size_t i = 0, j = 1; i < octaves; ++i, j = 2 * j)
      {
         noise = noise + (float) (Perlin::noise(t[x] * j, t[y] * j, t[z] * j) / j);
      }

      noise = noise + (float) sin(frequency * sqrt(t[x] * t[x] + t[y] * t[y]) * amplitude);

      return noise;
   }

private:
   float frequency;
   float amplitude;
};

#endif
