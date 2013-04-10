/**
 * @File:   CublicSpline.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Cubic Spline Interpolation
 */

#ifndef _CubicSpline_H_
#define _CubicSpline_H_

#include <vector>
#include "Vector2.h"
#include "Tools/Debug/NaoTHAssert.h"

class CubicSpline
{
public:
  CubicSpline() {}
  
  CubicSpline(const std::vector<Vector2<double> >& samples)
  {
    init(samples);
  }


  void init(const std::vector<Vector2<double> >& samples)
  {
    theSamples = samples;
    size_t n = samples.size();
    std::vector<double> h(n-1), b(n-1);
    for (size_t i=0; i<n-1; i++)
    {
        h[i] = samples[i+1].x - samples[i].x;
        b[i] = (samples[i+1].y - samples[i].y) / h[i];
    }

    // Gaussian Elimination
    std::vector<double> u(n, 0), v(n,0);
    u[1] = 2*(h[0] + h[1] );
    v[1] = 6*(b[1] - b[0] );
    for (size_t i = 2; i < n - 1; i++) {
      u[i] = 2 * (h[i - 1] + h[i]) - h[i - 1] * h[i - 1] / u[i - 1];
      v[i] = 6 * (b[i] - b[i - 1]) - h[i - 1] * v[i - 1] / u[i - 1];
    }

    // Back-substitution
    theZ = std::vector<double>(n, 0);
    for ( int i=n-2; i>0; i--)
    {
      theZ[i] = (v[i] - h[i] * theZ[i+1] ) / u[i];
    }
  }

  // calculate y by given x
  double y(double x)
  {
    int n = theSamples.size();
    ASSERT(n >= 3);
    ASSERT( theSamples.front().x <= x );
    ASSERT( x <= theSamples.back().x );

    
    for (int i = 0; i < n - 1; i++) {
      if (theSamples[i].x <= x && x <= theSamples[i + 1].x) {
        double h = theSamples[i + 1].x - theSamples[i].x;
        double a = theSamples[i].y;
        double b = -h * theZ[i + 1] / 6 - h * theZ[i] / 3 + (theSamples[i + 1].y - theSamples[i].y) / h;
        double c = theZ[i] / 2;
        double d = (theZ[i + 1] - theZ[i]) / (6 * h);
        x -= theSamples[i].x;
        return a + x * (b + x * (c + x * d));
      }
    }
    
    ASSERT(false);
    return 0;
  }

private:
  std::vector<Vector2<double> > theSamples;
  std::vector<double> theZ;
};

#endif  /* _CubicSpline_H_ */

