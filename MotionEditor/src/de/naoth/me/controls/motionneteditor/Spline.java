/*
 * 
 */
package de.naoth.me.controls.motionneteditor;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Path2D;
import java.awt.geom.Path2D.Double;

/**
 *
 * @author Heinrich Mellmann
 */
public class Spline
{

  private Point[] points;
  private Path2D.Double path;
  private double numberOfInterpolationPoints = 100;

  public Spline(int[] xArray, int[] yArray)
  {
    points = new Point[xArray.length];
    for (int j = 0; j < xArray.length; j++)
    {
      points[j] = new Point(xArray[j], yArray[j]);
    }
    path = new Path2D.Double();
    setPath();
  }

  /**
   *
   */
  private void setPath()
  {
    path.reset();
    int n = points.length;

    for (int j = 0; j <= numberOfInterpolationPoints; j++)
    {
      double t = (double) j / numberOfInterpolationPoints; // [0 <= t <= 1.0]
      double x = 0;
      double y = 0;
      for (int k = 0; k < n; k++)
      {
        x += B(n - 1, k, t) * points[k].x;
        y += B(n - 1, k, t) * points[k].y;
      }
      if (j > 0)
      {
        path.lineTo(x, y);
      }
      else
      {
        path.moveTo(x, y);
      }
    }//end for
  }//end setPath

  private double B(int n, int m, double t)
  {
    return binomialCoefficient(n, m) * Math.pow(t, m) * Math.pow(1.0 - t, n - m);
  }//end B

  private long binomialCoefficient(long n, long k)
  {
    if (k > n / 2)
    {
      k = n - k;
    }
    if (k <= 0 || n <= 0)
    {
      return 1;
    }
    else
    {
      return binomialCoefficient(n - 1, k - 1) * n / k;
    }
  }//end binomialCoefficient

  public void draw(Graphics2D g2d)
  {
    //g2d.setStroke(new BasicStroke(1f));
    g2d.draw(path);

    // draw the interpolated points
    /*
    g2d.setPaint(Color.red);
    for(int j = 0; j < points.length; j++) {
        mark(g2d, points[j]);
    }
    */
  }//end draw

  private void mark(Graphics2D g2, Point p)
  {
      g2.fill(new Ellipse2D.Double(p.x-1, p.y-1, 2, 2));
  }

  public Double getPath()
  {
    return path;
  }

}//end class Spline
