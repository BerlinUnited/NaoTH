/**
* @author mellmann@informatik.hu-berlin.de
*/
#ifndef _ImagePrimitives_h_
#define _ImagePrimitives_h_

// TODO: rename to AlignedPixel or so
template <typename T>
union PixelT
{
  // representation of the YUV422 pixel format 4 bite: |y0|u|y1|v|
  struct
  {
    T y0; /** Brightness 1*/ 
    T u;  /** U, also known as Cb */
    T y1; /** Brightness 2*/
    T v;  /** V, also known as Cr */
  };

  struct
  {
    T _yPaddingIgnore_; /** Brightness 2*/
    T cb; /** U, also known as Cb */
    T y; /** Brightness 2*/
    T cr; /** V, also known as Cr */
  };

  struct
  {
    T a;
    T b;
    T c;
    T d;
  };

  T channels[4];
};


typedef PixelT<unsigned char> Pixel; // default pixel type
typedef PixelT<double> DoublePixel;

#endif
