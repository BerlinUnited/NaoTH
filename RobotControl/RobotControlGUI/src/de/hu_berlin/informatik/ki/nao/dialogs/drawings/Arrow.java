/*
 * 
 */

package de.hu_berlin.informatik.ki.nao.dialogs.drawings;

import java.awt.BasicStroke;
import java.awt.Graphics2D;
import java.awt.Polygon;

/**
 *
 * @author Heinrich Mellmann
 */
public class Arrow implements Drawable
{
    private int x0;
    private int y0;
    private int x1;
    private int y1;

    public Arrow(
            int x0,
            int y0,
            int x1,
            int y1)
    {
        this.x0 = x0;
        this.y0 = y0;
        this.x1 = x1;
        this.y1 = y1;
    }
    
    public Arrow(String[] args)
    {
        this.x0 = (int)(Double.parseDouble(args[1]));
        this.y0 = (int)(Double.parseDouble(args[2]));
        this.x1 = (int)(Double.parseDouble(args[3]));
        this.y1 = (int)(Double.parseDouble(args[4]));
    }
    
    public void draw(Graphics2D g2d)
    {
        BasicStroke stroke = (BasicStroke)g2d.getStroke();
        float lineWidth = stroke.getLineWidth();
        drawArrow(g2d, x0, y0, x1, y1, lineWidth);
        g2d.setStroke(stroke);
    }//end draw
    
    public static void drawArrow(Graphics2D g2d, int xCenter, int yCenter, int x, int y, float stroke) {
      int size = (int)(stroke);
      double aDir=Math.atan2(xCenter-x, yCenter-y);
      g2d.setStroke(new BasicStroke(stroke));
      g2d.drawLine(x,y, xCenter,yCenter);
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
}//end class Arrow
