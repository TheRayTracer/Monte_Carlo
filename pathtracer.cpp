/* File: raytracer.cpp; Mode: C++; Tab-width: 3; Author: Simon Flannery;      */

#include "pathtracer.h"
#include "scene.h"
#include "object.h"
#include "material.h"
#include "pdf.h"

color3f PathTracer::TracePath(const Ray& ray, size_t bounce) const
{
   color3f color;

   if (bounce > max_bounces)
   {
      return color;
   }

   Hit hit;
   if (scene->GetGroup()->Intersect(ray, hit, epsilon) != false)
   {
      Material* material = hit.GetMaterial();

      color3f light = material->Emitted(hit.GetIntersectionPoint());

      vector3f scattered;
      if (material->Scatter(ray, hit, scattered) != false)
      {
         if (material->IsSpecular(hit.GetIntersectionPoint()) != false)
         {
            Ray specular_ray = Ray(hit.GetIntersectionPoint(), scattered);

            color = material->GetColor(hit.GetIntersectionPoint()) * TracePath(specular_ray, bounce + 1);
         }
         else
         {
            CosinePdf pdf(hit.GetNormal());
            Ray scatter_ray = Ray(hit.GetIntersectionPoint(), pdf.Generate());

         // float v = pdf.GetValue(scatter_ray.GetDirection());

         // color = light + material->GetColor(hit.GetIntersectionPoint()) * material->ScatterPdf(hit, scatter_ray.GetDirection()) * TracePath(scatter_ray, bounce + 1) / v;

            color = light + material->GetColor(hit.GetIntersectionPoint()) * TracePath(scatter_ray, bounce + 1);
         }
      }
      else
      {
         color = light;
      }
   }
   else
   {
      color = scene->GetBackground();
   }

   return color;
}
