/* File: perlin.h; Mode: C++; Tab-width: 3; Author: Ken Perlin;               */

#ifndef PERLIN_H
#define PERLIN_H

#include <math.h>
#include <stdio.h>

class Perlin
{
public:
   static float noise(float x, float y, float z)
   {
      int X = (int) floor(x) & 255;                  // FIND UNIT CUBE THAT
      int Y = (int) floor(y) & 255;                  // CONTAINS POINT.
      int Z = (int) floor(z) & 255;
      x -= floor(x);                                // FIND RELATIVE X,Y,Z
      y -= floor(y);                                // OF POINT IN CUBE.
      z -= floor(z);

      float u = fade(x);                                // COMPUTE FADE CURVES
      float v = fade(y);                                // FOR EACH OF X,Y,Z.
      float w = fade(z);

      int A = p[X    ] + Y; int AA = p[A] + Z; int AB = p[A + 1] + Z;      // HASH COORDINATES OF
      int B = p[X + 1] + Y; int BA = p[B] + Z; int BB = p[B + 1] + Z;      // THE 8 CUBE CORNERS,

      return lerp(w, lerp(v, lerp(u, grad(p[AA    ], x    , y    , z    ),  // AND ADD
                                     grad(p[BA    ], x - 1, y    , z    )), // BLENDED
                             lerp(u, grad(p[AB    ], x    , y - 1, z    ),  // RESULTS
                                     grad(p[BB    ], x - 1, y - 1, z    ))),// FROM  8
                     lerp(v, lerp(u, grad(p[AA + 1], x    , y    , z - 1),  // CORNERS
                                     grad(p[BA + 1], x - 1, y    , z - 1)), // OF CUBE
                             lerp(u, grad(p[AB + 1], x    , y - 1, z - 1),
                                     grad(p[BB + 1], x - 1, y - 1, z - 1))));
   }

protected:
private:
   static float fade(float t)
   { 
      return t * t * t * (t * (t * 6 - 15) + 10);
   }

   static float lerp(float t, float a, float b)
   { 
      return a + t * (b - a);
   }
   
   static float grad(int hash, float x, float y, float z)
   {
      int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE

      float u = h < 8 ? x : y;                 // INTO 12 GRADIENT DIRECTIONS.
      float v = h < 4 ? y : h == 12 || h == 14 ? x : z;

      return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
   }

/* Permutation. */
   static int p[512];
};

#endif
