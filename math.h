/* File: math.h; Mode: C++; Tab-width: 3; Author: Simon Flannery;             */

#ifndef MATH_H
#define MATH_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

enum {r, g, b};

enum {x, y, z, w};

#define PI 3.1415926535897932384626433832795f

inline float random_float() // [0, 1]
{
   return rand() / (RAND_MAX + 1.0f);
}

inline float random_float(float mi, float ma) // [mi, ma)
{
   return mi + (ma - mi) * random_float();
}

struct vector4f
{
   float m[4];

   vector4f(float a = 0.0f, float b = 0.0f, float c = 0.0f, float d = 0.0f)
   {
      m[x] = a;
      m[y] = b;
      m[z] = c;
      m[w] = d;
   }

   float& operator[](size_t i)
   { 
      return m[i];
   }

   float operator[](size_t i) const
   { 
      return m[i];
   }
};

struct vector3f
{
   float m[3];

   vector3f(float a = 0.0f, float b = 0.0f, float c = 0.0f)
   {
      m[x] = a;
      m[y] = b;
      m[z] = c;
   }

   vector3f& Normalize()
   {
      float length = (float) sqrt(m[x] * m[x] + m[y] * m[y] + m[z] * m[z]);
      m[x] = m[x] / length;
      m[y] = m[y] / length;
      m[z] = m[z] / length;
      
      return *this;
   }

   vector3f& Clamp(float low = 0.0f, float high = 1.0f)
   {
      if (m[x] < low) m[x] = low;  if (m[x] > high) m[x] = high;
      if (m[y] < low) m[y] = low;  if (m[y] > high) m[y] = high;
      if (m[z] < low) m[z] = low;  if (m[z] > high) m[z] = high;

      return *this;
   }

   float Length() const
   {
      return (float) sqrt(m[x] * m[x] + m[y] * m[y] + m[z] * m[z]);
   }

   float LengthSq() const
   {
      return m[x] * m[x] + m[y] * m[y] + m[z] * m[z];
   }

   vector3f& Negate()
   {
      m[x] = -m[x];
      m[y] = -m[y];
      m[z] = -m[z];

      return *this;
   }

   vector3f& Abs()
   {
      m[x] = (float) fabs(m[x]);
      m[y] = (float) fabs(m[y]);
      m[z] = (float) fabs(m[z]);

      return *this;
   }

   bool IsNearZero() const
   {
      const auto s = 1e-8;
      // Return true if the vector is close to zero in all dimensions.
      return (fabs(m[x]) < s) && (fabs(m[y]) < s) && (fabs(m[z]) < s);
   }

   void Set(float a = 0.0f, float b = 0.0f, float c = 0.0f)
   {
      m[x] = a;
      m[y] = b;
      m[z] = c;
      
      return;
   }

   float& operator[](size_t i)
   { 
      return m[i];
   }

   float operator[](size_t i) const
   { 
      return m[i];
   }

   friend vector3f operator - (const vector3f& a)
   {
      vector3f v(-a[x], -a[y], -a[z]);

      return v;
   }

   friend vector3f operator * (const vector3f& a, const float f)
   {
      vector3f v(a[x] * f, a[y] * f, a[z] * f);

      return v;
   }

   friend vector3f operator * (const float f, const vector3f& a)
   {
      vector3f v(a[x] * f, a[y] * f, a[z] * f);

      return v;
   }

   friend vector3f operator * (const vector3f& a, const vector3f& b)
   {
      vector3f v(a[x] * b[x], a[y] * b[y], a[z] * b[z]);

      return v;
   }

   friend vector3f operator / (const vector3f& a, const float f)
   {
      vector3f v(a[x] / f, a[y] / f, a[z] / f);
    
      return v;
   }

   friend vector3f operator + (const vector3f& a, const vector3f& b)
   {
      vector3f v(a[x] + b[x], a[y] + b[y], a[z] + b[z]);

      return v;
   }

   friend vector3f operator - (const vector3f& a, const vector3f& b)
   {
      vector3f v(a[x] - b[x], a[y] - b[y], a[z] - b[z]);

      return v;
   }

   friend bool operator > (const vector3f& a, const vector3f& b)
   {
      return a[x] > b[x] && a[y] > b[y] && a[z] > b[z];
   }

   static float Distance(const vector3f a, const vector3f b)
   {
      return (float) sqrt((a[x] - b[x]) * (a[x] - b[x]) + (a[y] - b[y]) * (a[y] - b[y]) + (a[z] - b[z]) * (a[z] - b[z]));
   }

   static float DistanceSq(const vector3f a, const vector3f b)
   {
      return (a[x] - b[x]) * (a[x] - b[x]) + (a[y] - b[y]) * (a[y] - b[y]) + (a[z] - b[z]) * (a[z] - b[z]);
   }

   static vector3f Cross(const vector3f& a, const vector3f& b)
   {
      vector3f v;

      v[x] = a[y] * b[z] - a[z] * b[y];
      v[y] = a[z] * b[x] - a[x] * b[z];
      v[z] = a[x] * b[y] - a[y] * b[x];

      return v;
   }

   static float Dot(const vector3f& a, const vector3f& b)
   {
      return a[x] * b[x] + a[y] * b[y] + a[z] * b[z];
   }

   static vector3f RandomInHemisphere(const vector3f& normal)
   {
      vector3f v = RandomVectorOnUnitSphere();

      if (vector3f::Dot(v, normal) <= 0.0f) // Is not in the same hemisphere as the normal.
      {
         return v.Negate();
      }

      return v;
   }

   static vector3f RandomVectorOnUnitSphere()
   {
      vector3f p;

      do
      {
         p = 2.0f * vector3f(random_float(), random_float(), random_float()) - vector3f(1.0f, 1.0f, 1.0f);
      } while (p.LengthSq() >= 1.0f);

      return p.Normalize();
   }

   static void ConstructBasisFromSingleVector(const vector3f& a, vector3f& w, vector3f& u, vector3f& v)
   {
      w = a;
      w.Normalize();

      vector3f t = w;

      if (fabs(t[x]) < fabs(t[y]) && fabs(t[x]) < fabs(t[z]))
      {
         t[x] = 1.0f;
      }
      else if (fabs(t[y]) < fabs(t[x]) && fabs(t[y]) < fabs(t[z]))
      {
         t[y] = 1.0f;
      }
      else if (fabs(t[z]) < fabs(t[y]) && fabs(t[z]) < fabs(t[x]))
      {
         t[z] = 1.0f;
      }

      u = vector3f::Cross(t, w);
      u.Normalize();

      v = vector3f::Cross(w, u);
   // v.Normalize();

      return;
   }
};

typedef vector3f point3f;
typedef vector3f color3f;

struct vector2f
{
   float m[2];

   vector2f(float a = 0.0f, float b = 0.0f)
   {
      m[x] = a;
      m[y] = b;
   }

   float& operator[](size_t i)
   { 
      return m[i];
   }

   float operator[](size_t i) const
   { 
      return m[i];
   }
};

typedef vector2f point2f;

inline float Det2x2(float a, float b,
                    float c, float d)
{
   return a * d - b * c;
}

inline float Det3x3(float a1,float a2,float a3,
                    float b1,float b2,float b3,
                    float c1,float c2,float c3)
{
   return
      a1 * Det2x2(b2, b3, c2, c3)
    - b1 * Det2x2(a2, a3, c2, c3)
    + c1 * Det2x2(a2, a3, b2, b3);
}

inline float Det4x4(float a1, float a2, float a3, float a4,
                    float b1, float b2, float b3, float b4, 
                    float c1, float c2, float c3, float c4, 
                    float d1, float d2, float d3, float d4)
{
   return 
      a1 * Det3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4)
    - b1 * Det3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4)
    + c1 * Det3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4)
    - d1 * Det3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

struct Matrix
{
   float data[4][4];

   Matrix()
   {
      Clear();
   }

   void Clear()
   {
      for (size_t k = 0; k < 4; ++k)
      {
         for (size_t j = 0; j < 4; ++j)
         {
            data[k][j] = 0.0f; 
         }
      }

      return;
   }

   void SetToIdentity()
   {
      for (size_t k = 0; k < 4; ++k)
      {
         for (size_t j = 0; j < 4; ++j)
         {
            data[k][j] = (j == k); 
         }
      }

      return;
   }

   void Set(size_t _x, size_t _y, float v)
   {
      data[_y][_x] = v;
     
      return;
   }

   void Transform(vector4f& v) const
   {
      vector4f answer;

      for (size_t k = 0; k < 4; ++k)
      {
         answer[k] = 0.0f;

         for (size_t i = 0; i < 4; ++i)
         {
            answer[k] += data[k][i] * v[i];
         }
      }
      
      v = answer;

      return;
   }

   void Transform(vector3f& v) const
   {
      vector4f v2(v[x], v[y], v[z], 1.0f);
      
      Transform(v2);
    
      v.Set(v2[x], v2[y], v2[z]);

      return;
   }

   void TransformDirection(vector3f& v) const
   {
      vector4f v2(v[x], v[y], v[z], 0.0f);

      Transform(v2);
      
      v.Set(v2[x], v2[y], v2[z]);

      return;
   }

   Matrix Inverse() const
   {
      Matrix m = *this;

      float a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;
      a1 = m.data[0][0]; b1 = m.data[0][1]; c1 = m.data[0][2]; d1 = m.data[0][3];
      a2 = m.data[1][0]; b2 = m.data[1][1]; c2 = m.data[1][2]; d2 = m.data[1][3];
      a3 = m.data[2][0]; b3 = m.data[2][1]; c3 = m.data[2][2]; d3 = m.data[2][3];
      a4 = m.data[3][0]; b4 = m.data[3][1]; c4 = m.data[3][2]; d4 = m.data[3][3];

      float det = Det4x4(a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4);

      m.data[0][0] =  Det3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
      m.data[1][0] = -Det3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
      m.data[2][0] =  Det3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
      m.data[3][0] = -Det3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

      m.data[0][1] = -Det3x3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
      m.data[1][1] =  Det3x3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
      m.data[2][1] = -Det3x3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
      m.data[3][1] =  Det3x3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

      m.data[0][2] =  Det3x3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
      m.data[1][2] = -Det3x3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
      m.data[2][2] =  Det3x3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
      m.data[3][2] = -Det3x3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

      m.data[0][3] = -Det3x3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
      m.data[1][3] =  Det3x3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
      m.data[2][3] = -Det3x3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
      m.data[3][3] =  Det3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3);

      return m * (1.0f / det);
   }

   Matrix Transpose() const
   {
      Matrix m;

      for (size_t k = 0; k < 4; ++k)
      {
         for (size_t j = 0; j < 4; ++j)
         {
            m.data[k][j] = data[j][k];
         }
      }

      return m;
   }

   friend Matrix operator * (const Matrix& m1, const Matrix& m2)
   {
      Matrix m;
      
      for (size_t k = 0; k < 4; ++k)
      {
         for (size_t j = 0; j < 4; ++j)
         {
            for (size_t i = 0; i < 4; ++i)
            {
               m.data[k][j] += m1.data[k][i] * m2.data[i][j];
            }
         }
      }

      return m;
   }

   friend Matrix operator * (const Matrix& m1, const float f)
   {
      Matrix m;
      
      for (size_t k = 0; k < 4; ++k)
      {
         for (size_t j = 0; j < 4; ++j)
         {
            m.data[k][j] = m1.data[k][j] * f;
         }
      }

      return m;
   }

   static Matrix MakeTranslation(const vector3f& v)
   {
      Matrix t;
      t.SetToIdentity();
      t.data[0][3] = v[x];
      t.data[1][3] = v[y];
      t.data[2][3] = v[z];

      return t;
   }

   static Matrix MakeScale(const vector3f& v)
   {
      Matrix s; 
      s.SetToIdentity();
      s.data[0][0] = v[x];
      s.data[1][1] = v[y];
      s.data[2][2] = v[z];
      s.data[3][3] = 1.0f;

      return s;
   }

   static Matrix MakeXRotation(float theta)
   {
      Matrix rx;
      rx.SetToIdentity();
      rx.data[1][1] =  (float) cos(theta);
      rx.data[1][2] = -(float) sin(theta);
      rx.data[2][1] =  (float) sin(theta);
      rx.data[2][2] =  (float) cos(theta);
      
      return rx;
   }

   static Matrix MakeYRotation(float theta)
   {
      Matrix ry;
      ry.SetToIdentity();
      ry.data[0][0] =  (float) cos(theta);
      ry.data[0][2] =  (float) sin(theta);
      ry.data[2][0] = -(float) sin(theta);
      ry.data[2][2] =  (float) cos(theta);
      
      return ry;
   }

   static Matrix MakeZRotation(float theta)
   {
      Matrix rz;
      rz.SetToIdentity();
      rz.data[0][0] =  (float) cos(theta);
      rz.data[0][1] = -(float) sin(theta);
      rz.data[1][0] =  (float) sin(theta);
      rz.data[1][1] =  (float) cos(theta);
      
      return rz;
   }

   static Matrix MakeAxisRotation(const vector3f& v, float theta)
   {
      Matrix r;
      r.SetToIdentity();

      float fx = v[x];
      float fy = v[y];
      float fz = v[z];

      float c = (float) cos(theta);
      float s = (float) sin(theta);
      
      float xx = fx * fx;
      float xy = fx * fy;
      float xz = fx * fz;
      float yy = fy * fy;
      float yz = fy * fz;
      float zz = fz * fz;

      r.data[0][0] = (1.0f - c) * xx + c;
      r.data[0][1] = (1.0f - c) * xy + fz * s;
      r.data[0][2] = (1.0f - c) * xz - fy * s;
      r.data[0][3] = 0.0f;

      r.data[1][0] = (1.0f - c) * xy - fz * s;
      r.data[1][1] = (1.0f - c) * yy + c;
      r.data[1][2] = (1.0f - c) * yz + fx * s;
      r.data[1][3] = 0.0f;

      r.data[2][0] = (1.0f - c) * xz + fy * s;
      r.data[2][1] = (1.0f - c) * yz - fx * s;
      r.data[2][2] = (1.0f - c) * zz + c;
      r.data[2][3] = 0.0f;

      r.data[3][0] = 0.0f;
      r.data[3][1] = 0.0f;
      r.data[3][2] = 0.0f;
      r.data[3][3] = 1.0f;

      return r;
   }
};

#endif
