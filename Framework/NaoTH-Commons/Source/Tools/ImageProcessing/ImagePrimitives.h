/**
* @author edel at informatik.hu-berlin.de
*/
#ifndef __ImagePrimitives_h_
#define __ImagePrimitives_h_

union Pixel
{
  struct
  {
    /** Brightness */
    unsigned char y;
    /** U, also known as Cb */
    unsigned char u;
    /** V, also known as Cr */
    unsigned char v;
  };

  struct
  {
    unsigned char a;
    unsigned char b;
    unsigned char c;
  };

  unsigned char channels[3];

};

struct Point {
  unsigned int x;
  unsigned int y;
};

union DoublePixel
{
  struct
  {
    /** Y, also known as Luma (Brightness) */
    double y;
    /** U, also known as Chroma blue (Cb)*/
    double u;
    /** V, also known as Chroma red (Cr) */
    double v;
  };

  struct
  {
    double a;
    double b;
    double c;
  };

  double channels[3];

};

union IntPixel
{
  struct
  {
    /** Y, also known as Luma (Brightness) */
    int y;
    /** U, also known as Chroma blue (Cb)*/
    int u;
    /** V, also known as Chroma red (Cr) */
    int v;
  };

  struct
  {
    int a;
    int b;
    int c;
  };

  short channels[3];

};


#endif
