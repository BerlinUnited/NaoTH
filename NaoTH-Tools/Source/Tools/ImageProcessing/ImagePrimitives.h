/**
* @author edel at informatik.hu-berlin.de
*/
#ifndef __ImagePrimitives_h_
#define __ImagePrimitives_h_

namespace naoth
{

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
}

#endif
