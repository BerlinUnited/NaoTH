/**
* @author edel at informatik.hu-berlin.de
*/
#ifndef _ImagePrimitives_h_
#define _ImagePrimitives_h_

template <typename T>
union PixelT
{
  struct
  {
    /** Brightness */
    T y;
    /** U, also known as Cb */
    T u;
    /** V, also known as Cr */
    T v;
  };

  struct
  {
    T a;
    T b;
    T c;
  };

  T channels[3];

};


typedef PixelT<unsigned char> Pixel; // default pixel type
typedef PixelT<double> DoublePixel;


struct Point {
  unsigned int x;
  unsigned int y;
};


#endif
