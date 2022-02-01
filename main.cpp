/* File: main.cpp; Mode: C++; Tab-width: 3; Author: Simon Flannery;           */

/*
   Simon Flannery, 2022 - 2022.
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include "math.h"
#include "camera.h"
#include "scene.h"
#include "pathtracer.h"
#include "image.h"

/* Render the scene to an image. */
void Biscuit(Scene* scene, const char* szImageFileName, const size_t width, const size_t height, size_t max_bounces, const float epsilon, const size_t samples_per_pixel);

int main(size_t argc, char* argv[])
{
   srand((unsigned int) time(NULL));

   size_t width = 0, height = 0, max_bounces = 0, samples_per_pixel = 10;
   float epsilon = EPSILON;
   char* szInputFileName = NULL, * szImageFileName = NULL;

   for (size_t i = 1; i < argc; ++i)
   {
      if (strcmp(argv[i], "-input") == 0)
      {
         ++i; assert(i < argc);
         szInputFileName = argv[i];
      }
      else if (strcmp(argv[i], "-size") == 0)
      {
         ++i; assert(i < argc);
         width = atoi(argv[i]);
         ++i; assert(i < argc);
         height = atoi(argv[i]);
      }
      else if (strcmp(argv[i], "-output") == 0)
      {
         ++i; assert(i < argc);
         szImageFileName = argv[i];
      }
      else if (strcmp(argv[i], "-samples") == 0)
      {
         ++i; assert(i < argc);
         samples_per_pixel = atoi(argv[i]);
      }
      else if (strcmp(argv[i], "-bounces") == 0)
      {
         ++i; assert(i < argc);
         max_bounces = atoi(argv[i]);
      }
      else if (strcmp(argv[i], "-epsilon") == 0)
      {
         ++i; assert(i < argc);
         epsilon = (float) atof(argv[i]);
      }
   }

   Scene* scene = new Scene(szInputFileName);

   auto start_time = time(NULL);

   Biscuit(scene, szImageFileName, width, height, max_bounces, epsilon, samples_per_pixel);

   auto finish_time = time(NULL);

   auto delta_time = finish_time - start_time;

   if (delta_time == 0)
   {
      delta_time = 1;
   }

   int seconds = (int) (delta_time % 60);
   int minutes = (int) ((delta_time / 60) % 60);
   int hours = (int) (delta_time / (60 * 60));

   printf("\n\n");
   printf("Rendering time: %02d:%02d:%02d\n", hours, minutes, seconds);

   delete scene;

   return 0;
}

void Biscuit(Scene* scene, const char* szImageFileName, const size_t width, const size_t height, size_t max_bounces, const float epsilon, const size_t samples_per_pixel)
{
   PathTracer* trace = new PathTracer(scene, max_bounces);

   Camera* camera = scene->GetCamera();

   if (camera != NULL && scene->GetGroup() != NULL)
   {
      const size_t num_pixels = height * width;
      int last_percent = 0;

      Image capture(width, height);

      for (size_t i = 0; i < width; ++i)
      {
         for (size_t j = 0; j < height; ++j)
         {
            float pc = (float) (i * width + j);
            int percent = (int) (100.0f * (pc / num_pixels));

            if (percent != last_percent)
            {
               printf("%2d%c ", percent, '%'); fflush(NULL);

               last_percent = percent;
            }

            color3f color;

            for (size_t t = 0; t < samples_per_pixel; ++t)
            {
               point2f jitter(random_float(), random_float());

               point2f p((i + jitter[x]) / (float) width,
                         (j + jitter[y]) / (float) height);

               const Ray ray = camera->GenerateRay(p);

               color3f color_contribution = trace->TracePath(ray, 0);

               color = color + color_contribution;
            }

            const float s = 1.0f / (float) samples_per_pixel;

            color[r] = (float) sqrt(color[r] * s);
            color[g] = (float) sqrt(color[g] * s);
            color[b] = (float) sqrt(color[b] * s);

            capture.SetPixel(i, j, color);
         }
      }

      Image::Format type = Image::Format::TGA;

      if (strstr(szImageFileName, ".bmp") != NULL)
      {
         type = Image::Format::BMP;
      }

      capture.Save(szImageFileName, type);
   }

   delete trace;

   return;
}
