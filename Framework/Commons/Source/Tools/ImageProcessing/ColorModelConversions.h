/**
 * @file ColorModelConversions.h
 * 
 * Declaration and implementation of class ColorModelConversions
 */ 

#ifndef _ColorModelConversions_h_
#define _ColorModelConversions_h_

#include "Tools/Math/Common.h"

namespace naoth
{

  /**
  * A class that defines static inline conversions between color models for single pixels.
  */
  class ColorModelConversions
  {
  public:
    /** Converts an YCbCr pixel into an RGB pixel.
     *  taken from http://msdn.microsoft.com/en-us/library/ms893078.aspx
     *  @param Y The Y channel of the source pixel.
     *  @param Cb The Cb channel of the source pixel.
     *  @param Cr The Cr channel of the source pixel.
     *  @param R The R channel of the target pixel.
     *  @param G The G channel of the target pixel.
     *  @param B The B channel of the target pixel.
     */
    static inline void fromYCbCrToRGB(unsigned char Y,
                               unsigned char Cb,
                               unsigned char Cr,
                               unsigned char& R,
                               unsigned char& G,
                               unsigned char& B)
    {
      int r = ( 298 * (Y - 16)                    + 409 * (Cr - 128) + 128) >> 8,
          g = ( 298 * (Y - 16) - 100 * (Cb - 128) - 208 * (Cr - 128) + 128) >> 8,
          b = ( 298 * (Y - 16) + 516 * (Cb - 128)                    + 128) >> 8;
      if(r < 0) r = 0; else if(r > 255) r = 255;
      if(g < 0) g = 0; else if(g > 255) g = 255;
      if(b < 0) b = 0; else if(b > 255) b = 255;
      R = (unsigned char) r;
      G = (unsigned char) g;
      B = (unsigned char) b;
    }

    /** Converts an RGB pixel into an YCbCr pixel.
     *  taken from http://msdn.microsoft.com/en-us/library/ms893078.aspx
     *  @param R The R channel of the source pixel.
     *  @param G The G channel of the source pixel.
     *  @param B The B channel of the source pixel.
     *  @param Y The Y channel of the target pixel.
     *  @param Cb The Cb channel of the target pixel.
     *  @param Cr The Cr channel of the target pixel.
     */
    static inline void fromRGBToYCbCr(unsigned char R,
                               unsigned char G,
                               unsigned char B,
                               unsigned char& Y,
                               unsigned char& Cb,
                               unsigned char& Cr)
    {
      int y  = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16,
          cb = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128,
          cr = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;
      if(y < 0) y = 0; else if(y > 255) y = 255;
      if(cb < 0) cb = 0; else if(cb > 255) cb = 255;
      if(cr < 0) cr = 0; else if(cr > 255) cr = 255;
      Y = (unsigned char) y;
      Cb = (unsigned char) cb;
      Cr = (unsigned char) cr;
    }


    /** Converts an YCbCr pixel into an HSI pixel.
     *  @param Y The Y channel of the source pixel.
     *  @param U The Cb channel of the source pixel.
     *  @param V The Cr channel of the source pixel.
     *  @param H The H channel of the target pixel.in 0-360 degree
     *  @param S The S channel of the target pixel.in 0-1000 1/10 percent
     *  @param I The I channel of the target pixel.in 0-1000 1/10 percent
     */
    static inline void fromYUVToHSI(unsigned char Y,
                               unsigned char U,
                               unsigned char V,
                               unsigned int& H,
                               unsigned int& S,
                               unsigned int& I)
    {
      //Y berechnen und auf [0,1] normalisieren
      double y = ((double) Y) / 255.0;
      //U berechnen und auf [-0.436,0.436] normalisieren
  //    double u = (((double) U) / 255.0) * 0.872 - 0.436;
      double u = (((double) U) / 127.5 - 1) * 0.436;//255.0;
      if(u < -0.436)
      {
        u = -0.436;
      }
      if(u > 0.436)
      {
        u = 0.436;
      }
      //V berechnen und auf [-0.615,0.615] normalisieren
  //    double v = (((double) V) / 255.0) * 1.230 - 0.615;
      double v = (((double) V) / 127.5 - 1) * 0.615;//255.0;
      if(v < -0.615)
      {
        v = -0.615;
      }
      if(v > 0.615)
      {
        v = 0.615;
      }

      //R berechnen und auf [0,1] normalisieren
      double r = (y + 1.140 * v);// - 0.299) / 1.4021;//0.299 - 1.7011
      if(r < 0.0)
      {
        r = 0.0;
      }
      if(r > 1.0)
      {
        r = 1.0;
      }
      //G berechnen und auf [0,1] normalisieren
      double g = (y - 0.394 * u - 0.580 * v);// - 0.472);//0.472 - 1.472
      if(g < 0.0)
      {
        g = 0.0;
      }
      if(g > 1.0)
      {
        g = 1.0;
      }
      //B berechnen und auf [0,1] normalisieren
      double b = (y + 2.032 * u - 0.114);// / 1.772;//0.114 - 1.886;
      if(b < 0.0)
      {
        b = 0.0;
      }
      if(b > 1.0)
      {
        b = 1.0;
      }

      double m = r < g ? (r < b ? r : (b < g ? b : g)) : g;
      double M = r > g ? (r > b ? r : (b > g ? b : g)) : g;

      double c = M - m;

      double i = (r + b + g) / 3;

      double s = 1;

      if(i > 0.0)
      {
        s -= (m / i) ;
      }

      if(s < 0.0)
      {
        s = 0.0;
      }

      double h = 0.0;

      if(c > 0.0)
      {
        if(M == r)
        {
          h = ((int) ceil((g - b) / c)) % 6;
        }
        else if(M == g)
        {
          h = ceil((b - r) / c) + 2;
        }
        else if(M == b)
        {
          h = ceil((r - g) / c) + 4;
        }
        h = h * 60;
      }

      while(h > 360.0)
      {
        h -= 360.0;
      }

      while(h < 0.0)
      {
        h += 360.0;
      }

      H = (unsigned int) h;
      S = (unsigned int) ceil(s * 1000);
      I = (unsigned int) ceil(i * 1000);
    }

    /** Converts an YCbCr pixel into an HSI pixel.
     *  @param Y The Y channel of the source pixel.
     *  @param Cb The Cb channel of the source pixel.
     *  @param Cr The Cr channel of the source pixel.
     *  @param H The H channel of the target pixel.
     *  @param S The S channel of the target pixel.
     *  @param I The I channel of the target pixel.
     */
    static inline void fromYCbCrToHSI(unsigned char Y,
                               unsigned char Cb,
                               unsigned char Cr,
                               unsigned char& H,
                               unsigned char& S,
                               unsigned char& I)
    {
      using namespace Math;
      const double sqrt3 = 1.732050808;
      unsigned char R,
                    G,
                    B;
      fromYCbCrToRGB(Y, Cb, Cr, R, G, B);
      I = R;
      if(G > I) I = G;
      if(B > I) I = B;
      if(I)
      {
        S = R;
        if(G < S) S = G;
        if(B < S) S = B;
        S = (unsigned char) (255 - 255 * S / I);
        if(S)
        {
          int h = int(atan2(sqrt3 * (G - B), 2 * R - G - B) / pi2 * 256);
          if(h > 255) h -= 256;
          else if(h < 0) h += 256;
          H = (unsigned char) h;
        }
        else
          H = 0;
      }
      else
        S = H = 0;
    }

    /** Converts an HSI pixel into an YCbCr pixel.
     *  @param H The H channel of the source pixel.
     *  @param S The S channel of the source pixel.
     *  @param I The I channel of the source pixel.
     *  @param Y The Y channel of the target pixel.
     *  @param Cb The Cb channel of the target pixel.
     *  @param Cr The Cr channel of the target pixel.
     */
    static inline void fromHSIToYCbCr(unsigned char H,
                               unsigned char S,
                               unsigned char I,
                               unsigned char& Y,
                               unsigned char& Cb,
                               unsigned char& Cr)
    {
      using namespace Math;
      double h = 1.0 - H / 255.0,
             s = S / 255.0,
             r,
             g,
             b;

      if(h < 1.0 / 3.0)
      {
        g = (1 - s) / 3;
        b = (1 + s * cos(pi2 * h) / cos(pi2 * (1.0 / 6.0 - h))) / 3.0;
        r = 1 - (g + b);
      }
      else if(h < 2.0 / 3.0)
      {
        h -= 1.0 / 3.0;
        b = (1 - s) / 3;
        r = (1 + s * cos(pi2 * h) / cos(pi2 * (1.0 / 6.0 - h))) / 3.0;
        g = 1 - (b + r);
      }
      else
      {
        h -= 2.0 / 3.0;
        r = (1 - s) / 3;
        g = (1 + s * cos(pi2 * h) / cos(pi2 * (1.0 / 6.0 - h))) / 3.0;
        b = 1 - (r + g);
      }

      r *= I * 3;
      g *= I * 3;
      b *= I * 3;
      if(r > 255)
        r = 255;
      if(g > 255)
        g = 255;
      if(b > 255)
        b = 255;

      fromRGBToYCbCr((unsigned char) r,
                     (unsigned char) g,
                     (unsigned char) b,
                     Y, Cb, Cr);
    }

    /** Converts an YCbCr pixel into a TSL pixel.
     *  @param Y The Y channel of the source pixel.
     *  @param Cb The Cb channel of the source pixel.
     *  @param Cr The Cr channel of the source pixel.
     *  @param T The T channel of the target pixel.
     *  @param S The S channel of the target pixel.
     *  @param L The L channel of the target pixel.
     */
    static inline void fromYCbCrToTSL(unsigned char Y,
                               unsigned char Cb,
                               unsigned char Cr,
                               unsigned char& T,
                               unsigned char& S,
                               unsigned char& L)
    {
      const double pi2_div = 0.15915494309189533576888376337251;  /* 1.0 / (PI * 2.0) */

      double cb = Cb - 128.0,
             cr = Cr - 128.0,
             tmp = 1.0 / (4.3403 * Y +  2.0 * cb + cr),
             tmp_r = (-0.6697 * cb + 1.6959 * cr) * tmp,
             tmp_g = (-1.168  * cb - 1.3626 * cr) * tmp,
             tmp_b = ( 1.8613 * cb - 0.331  * cr) * tmp;
      double t_out;
      if(tmp_g > 0.0)
        t_out = (atan2(tmp_r, tmp_g) * pi2_div + 0.25) * 255.0;
      else if(tmp_g < 0.0)
        t_out = (atan2(-tmp_r, -tmp_g) * pi2_div + 0.75) * 255.0;
      else
        t_out = 0.0;
      double s_out = sqrt(1.8 * (tmp_r * tmp_r + tmp_g * tmp_g + tmp_b * tmp_b)) * 255.0;

      if(t_out < 0.0)
        t_out = 0.0;
      else if(t_out > 255.0)
        t_out = 255.0;
      if(s_out < 0.0)
        s_out = 0.0;
      else if(s_out > 255.0)
        s_out = 255.0;

      T = (unsigned char) t_out;
      S = (unsigned char) s_out;
      L = Y;
    }

    /** Converts a TSL pixel into an YCbCr pixel.
     *  @param T The T channel of the source pixel.
     *  @param S The S channel of the source pixel.
     *  @param L The L channel of the source pixel.
     *  @param Y The Y channel of the target pixel.
     *  @param Cb The Cr channel of the target pixel.
     *  @param Cr The Cr channel of the target pixel.
     */
    static inline void fromTSLToYCbCr(unsigned char T,
                               unsigned char S,
                               unsigned char L,
                               unsigned char& Y,
                               unsigned char& Cb,
                               unsigned char& Cr)
    {
      using namespace Math;
      double rad = S * 0.555 / 255.0,
             phi = (T * 2 / 255.0 + 0.25) * pi,
             cr = 128 + rad * cos(phi) * 255.0,
             cb = 128 - rad * sin(phi) * 255.0;
      Y = L;

      if(cb < 0)
        cb = 0;
      else if(cb > 255)
        cb = 255;
      if(cr < 0)
        cr = 0;
      else if(cr > 255)
        cr = 255;
      Cb = (unsigned char) cb;
      Cr = (unsigned char) cr;
    }

    /** Converts a TSL pixel into an YCbCr pixel.
     *  @param T The T channel of the source pixel.
     *  @param S The S channel of the source pixel.
     *  @param L The L channel of the source pixel.
     *  @param Y The Y channel of the target pixel.
     *  @param Cb The Cr channel of the target pixel.
     *  @param Cr The Cr channel of the target pixel.
     */
    static inline void fromTSLToRGB(unsigned char t,
                             unsigned char s,
                             unsigned char l,
                             unsigned char& r,
                             unsigned char& g,
                             unsigned char& b)
    {
      double y1, u1, v1, r1, g1, b1, rad, phi;

      /* Convert TSL to YUV */
      rad = (((double) s) * 0.555) / 255.0;
      phi = (((double) t) * 2.0 * 3.14159) / 255.0;
      phi += 0.25 * 3.14159;
      y1 = (double) l;
      u1 =  (rad * cos(phi)) * 255.0;
      v1 = -(rad * sin(phi)) * 255.0;

      /* Crop UV */
      if (u1 < -128.0)
      {
        u1 = -128.0;
      }
      else if (u1 > 127.0)
      {
        u1 = 127.0;
      }
      if (v1 < -128.0)
      {
        v1 = -128.0;
      }
      else if (v1 > 127.0)
      {
        v1 = 127.0;
      }

      /* Convert YUV to RGB */
      r1 = y1 + 1.371 * v1;
      g1 = y1 - 0.336 * u1 - 0.698 * v1;
      b1 = y1 + 1.732 * u1;

      /* Crop RGB */
      if (r1 < 0.0)
      {
        r1 = 0.0;
      }
      else if (r1 > 255.0)
      {
        r1 = 255.0;
      }
      if (g1 < 0.0)
      {
        g1 = 0.0;
      }
      else if (g1 > 255.0)
      {
        g1 = 255.0;
      }
      if (b1 < 0.0)
      {
        b1 = 0.0;
      }
      else if (b1 > 255.0)
      {
        b1 = 255.0;
      }

      /* Return values */
      r = (unsigned char) r1;
      g = (unsigned char) g1;
      b = (unsigned char) b1;
    }
  };
}

#endif //__ColorModelConversions_h_
