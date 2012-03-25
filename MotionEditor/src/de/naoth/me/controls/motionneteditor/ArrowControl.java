/*
 * 
 */

package de.naoth.me.controls.motionneteditor;

import de.naoth.me.controls.motionneteditor.Spline;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Polygon;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.Shape;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import javax.swing.JComponent;

/**
 *
 * @author Heinrich Mellmann
 */
public class ArrowControl extends JComponent
{
    private Line2D line;
    private Spline spline;
    private Color color;

    double arrowDirection;
    double arrowX;
    double arrowY;
    
    public Point2D getP1() {
        return line.getP1();
    }

    public void setP1(Point2D P1) {
      this.line.setLine(P1, line.getP2());
      createSpline(P1.getX(), P1.getY(), line.getP2().getX(), line.getP2().getY());
    }
    
    public Point2D getP2() {
        return line.getP2();
    }

    public void setP2(Point2D P2) {
      this.line.setLine(line.getP1().getX(), line.getP1().getY(), P2.getX(), P2.getY());
      createSpline(line.getP1().getX(), line.getP1().getY(), P2.getX(), P2.getY());
    }
    
    private void createSpline(double x0, double y0, double x1, double y1)
    {
      double a = 100.0;
      double b = 70.0;

      double mx = (x1 + x0)*0.5;
      double my = (y1 + y0)*0.5;

      double vx = x1 - x0;
      double vy = y1 - y0;
      double vl = Math.sqrt(vx*vx + vy*vy);

      double maxLength = 200;
      double t = vl/maxLength;
      a = t*10.0 + (1-t)*150;
      b = t*10.0 + (1-t)*100;

      if(vl > 0)
      {
        vx /= vl;
        vy /= vl;
      }else
      {
        vx = 1.0;
        vy = 0.0;
        vl = 1.0;
      }

      // create orthogonal vector
      double wx = vy;
      double wy = -vx;

      double u = (vl - a)*0.5;

      int[] xPoints;
      int[] yPoints;
      
      if(u > b || t >= 1.0)
      {
        xPoints = new int[]{(int)x0, (int)x1};
        yPoints = new int[]{(int)y0, (int)y1};
      }else
      {
        double h = Math.sqrt(b*b - u*u);
        double px = mx + a*0.5*vx + h*wx;
        double py = my + a*0.5*vy + h*wy;

        double qx = mx - a*0.5*vx + h*wx;
        double qy = my - a*0.5*vy + h*wy;

        xPoints = new int[]{(int)x0, (int)qx, (int)px, (int)x1};
        yPoints = new int[]{(int)y0, (int)qy, (int)py, (int)y1};
      }

      {
        // modify the points in order to get endings on the border of keyframes
        double rx = xPoints[xPoints.length-2] - xPoints[xPoints.length-1];
        double ry = yPoints[yPoints.length-2] - yPoints[yPoints.length-1];
        double rl = Math.sqrt(rx*rx + ry*ry);

        this.arrowDirection = Math.atan2(rx, ry);
        
        rx /= rl;
        ry /= rl;

        double nodeRadius = 25.0;

        xPoints[xPoints.length-1] += rx*nodeRadius;
        yPoints[yPoints.length-1] += ry*nodeRadius;

        this.arrowX = xPoints[xPoints.length-1];
        this.arrowY = yPoints[yPoints.length-1];
      }

      {
        // modify the points in order to get endings on the border of keyframes
        double rx = xPoints[1] - xPoints[0];
        double ry = yPoints[1] - yPoints[0];
        double rl = Math.sqrt(rx*rx + ry*ry);

        rx /= rl;
        ry /= rl;

        double nodeRadius = 18.0;

        xPoints[0] += rx*nodeRadius;
        yPoints[0] += ry*nodeRadius;
      }
      
      this.spline = new Spline(xPoints, yPoints);
    }//end createSpline

    public Color getColor() {
        return color;
    }

    public void setColor(Color color) {
        this.color = color;
    }
    private BasicStroke stroke = new BasicStroke(10);
    private float drawingStrokeWidth;

    public float getDrawingStrokeWidth() {
        return drawingStrokeWidth;
    }

    public void setDrawingStrokeWidth(float drawingStrokeWidth) {
        this.drawingStrokeWidth = drawingStrokeWidth;
    }
    
    public ArrowControl()
    {
        this.color = Color.black;
        this.line = new Line2D.Double(0,0,0,0);
        this.drawingStrokeWidth = 1f;
        this.spline = new Spline(new int[]{0,0}, new int[]{0,0});
    }

    public Rectangle calculateBounds()
    {
        Shape outline = stroke.createStrokedShape(this.spline.getPath());
        return outline.getBounds();
    }//end getBounds
    
    /* Checks whether a Point cuts the Line */
    public boolean isContain(Point point)
    {
        Shape outline = stroke.createStrokedShape(this.spline.getPath());
        if (outline.contains(point)) 
        {
            return true;
        }
        else
            return false;
    }//end isContain
    
    @Override
    protected void paintComponent(Graphics g) 
    {
        Graphics2D g2d = (Graphics2D) g;

        // schalte antialiasing ein
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        g2d.setColor(this.color);

        g2d.setStroke(new BasicStroke(drawingStrokeWidth));
        this.spline.draw(g2d);
        
        g2d.setColor(this.color);
        drawArrow(g2d, (int)this.arrowX, (int)this.arrowY, this.arrowDirection, drawingStrokeWidth);
    }
    
    public static void drawArrow(Graphics2D g2d, int x, int y, double aDir, float stroke) {
      int size = (int)(stroke)+2;
      /*
      g2d.setStroke(new BasicStroke(stroke));
      g2d.drawLine(x,y, xCenter,yCenter);
      */
      g2d.setStroke(new BasicStroke(1f));

      x -= xCor(size*2,aDir);
      y -= yCor(size*2,aDir);

      int i1= size*2+(int)(stroke*2);
      int i2= size+(int)stroke;
      Polygon tmpPoly=new Polygon(
              new int[]{x, x+xCor(i1,aDir+.5), x+xCor(i2,aDir), x+xCor(i1,aDir-.5), x},
              new int[]{y, y+yCor(i1,aDir+.5), y+yCor(i2,aDir), y+yCor(i1,aDir-.5), y},
              5
              );
      g2d.drawPolygon(tmpPoly);
      g2d.fillPolygon(tmpPoly);
   }//end drawArrow
    
   private static int yCor(int len, double dir) {return (int)(len * Math.cos(dir));}
   private static int xCor(int len, double dir) {return (int)(len * Math.sin(dir));}
}//end clss ArrowControl
