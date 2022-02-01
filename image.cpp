/* File: image.cpp; Mode: C++; Tab-width: 3; Author: Simon Flannery;          */

#include "image.h"

#ifdef BITMAP_SUPPORT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

Image::Image(size_t width, size_t height) : w(width), h(height), data(NULL)
{
   size_t size = w * h;
   data = new vector3f[size];

   vector3f white(1.0f, 1.0f, 1.0f);

   for (size_t i = 0; i < size; ++i)
   {
      data[i] = white;
   }
}

Image::Image(const Image& i)
{
   w = i.w;
   h = i.w;

   size_t size = w * h;
   data = new vector3f[size];

   for (size_t j = 0; j < size; ++j)
   {
      data[j] = i.data[j];
   }
}

Image::~Image()
{
   delete [] data;
}

void Image::SetPixel(float _x, float _y, const color3f& color)
{
   if (_x < 0.0f)
   {
      _x = 0.0f;
   }
   else if (_x > 1.0f)
   {
      _x = 1.0f;
   }

   if (_y < 0.0f)
   {
      _y = 0.0f;
   }
   else if (_y > 1.0f)
   {
      _y = 1.0f;
   }

   size_t ix = (size_t) (_x * (float) w);
   size_t iy = (size_t) (_y * (float) h);
   
   data[iy * w + ix] = color;

   return;
}

void Image::SetPixel(size_t _x, size_t _y, const color3f& color)
{
   if (_x >= w)
   {
      _x = w - 1;
   }

   if (_y >= h)
   {
      _y = h - 1;
   }

   data[_y * w + _x] = color;

   return;
}

vector3f Image::GetPixel(size_t _x, size_t _y) const
{
   return data[_y * w + _x];
}

void Image::Save(const char* szFileName, Format t) const
{
   if (szFileName != NULL)
   {
      if (t == Format::TGA)
      {
         SaveTGA(szFileName);
      }
      else if (t == Format::BMP)
      {
         SaveBMP(szFileName);
      }
   }

   return;
}

void Image::SaveTGA(const char* szFileName) const
{
   FILE* file = fopen(szFileName, "wb");

   if (file != NULL)
   {
   /* Header information. */
      for (size_t i = 0; i < 18; ++i)
      {
         if (i == 2) WriteByte(file, 2);
         else if (i == 12) WriteByte(file, static_cast<unsigned char>(w % 256));
         else if (i == 13) WriteByte(file, static_cast<unsigned char>(w / 256));
         else if (i == 14) WriteByte(file, static_cast<unsigned char>(h % 256));
         else if (i == 15) WriteByte(file, static_cast<unsigned char>(h / 256));
         else if (i == 16) WriteByte(file, static_cast<unsigned char>(24));
         else if (i == 17) WriteByte(file, static_cast<unsigned char>(32));
         else WriteByte(file, 0);
      }

   /* Write the image data. */
      for (int j = h - 1; j >= 0; --j)
      {
         for (int i = 0; i < w; ++i)
         {
            color3f v = GetPixel(i, j);
            WriteByte(file, ClampColorComponent(v[b]));
            WriteByte(file, ClampColorComponent(v[g]));
            WriteByte(file, ClampColorComponent(v[r]));
         }
      }
      
      fclose(file);
   }

   return;
}

void Image::SaveBMP(const char* szFileName) const
{
#ifdef BITMAP_SUPPORT
   FILE* file = fopen(szFileName, "wb");

   if (file != NULL)
   {
      BITMAPFILEHEADER bmpHeader = {0};  /* Bitmap file header. */
             bmpHeader.bfSize    = sizeof(BITMAPFILEHEADER);
             bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
             bmpHeader.bfType    = BITMAP_ID; /* The magic number.  */

      size_t pad = (4 - (w * 3) % 4) % 4;

      BITMAPINFOHEADER bmpInfomation = {0}; /* Bitmap infomation header. */
      bmpInfomation.biSize          = sizeof(BITMAPINFOHEADER);
      bmpInfomation.biPlanes        = 1;  /* The only supported value. */
      bmpInfomation.biBitCount      = 24;
      bmpInfomation.biCompression   = BI_RGB;
      bmpInfomation.biWidth         = w;
      bmpInfomation.biHeight        = h;
      bmpInfomation.biSizeImage     = ((w * 3) + pad) * h;
      bmpInfomation.biXPelsPerMeter = 2834;
      bmpInfomation.biYPelsPerMeter = 2834;

      fwrite(&bmpHeader,     sizeof(BITMAPFILEHEADER), 1, file);
      fwrite(&bmpInfomation, sizeof(BITMAPINFOHEADER), 1, file);

   /* Write the image data upside-down. */
      for (size_t j = 0; j < h; ++j)
      {
         for (size_t i = 0; i < w; ++i)
         {
            color3f v = GetPixel(i, j);
            WriteByte(file, ClampColorComponent(v[b]));
            WriteByte(file, ClampColorComponent(v[g]));
            WriteByte(file, ClampColorComponent(v[r]));
         }

         for (size_t i = 0; i < pad; ++i)
         {
            WriteByte(file, 0);
         }
      }
      
      fclose(file);
   }
#endif
   return;
}

