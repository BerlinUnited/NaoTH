/*
 *
 */

package de.naoth.rc.dialogs.drawings;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;

/**
 *
 * @author Heinrich Mellmann
 */
public class RadarDrawing implements Drawable
{
  @Override
  public String toString() { return "Radar"; }
  
  @Override
  public void draw(Graphics2D g2d)
  {
    
    float backLight = 0.7f;
    float colorLight = 0.8f;

    // draw grid
    g2d.setColor(new Color(backLight, backLight, colorLight));
    double maxDist = 3000;
    double distStep = 500;
    for(double a = 500; a <= maxDist; a +=distStep)
    {
      g2d.drawOval((int)-a, (int)-a, (int)a*2, (int)a*2);
    }//end for

    
    g2d.setColor(new Color(backLight+0.1f, backLight+0.1f, colorLight));
    // small circles (every 100mm)
    for(double a = 100; a < 500; a +=100)
    {
      g2d.drawOval((int)-a, (int)-a, (int)a*2, (int)a*2);
    }//end for

    g2d.setColor(new Color(colorLight, backLight, backLight));
    double angleStep = 30.0*Math.PI/180.0;
    for(double a = -Math.PI; a < Math.PI; a +=angleStep)
    {
      g2d.drawLine(0, 0, (int)(Math.cos(a)*(maxDist+distStep)), (int)(Math.sin(a)*(maxDist+distStep)));
    }//end for

    
    g2d.scale(10, 10);
    g2d.rotate(-Math.PI*0.5);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));

    g2d.setColor(new Color(colorLight, backLight, backLight));
    double rotOffset = Math.PI;
    for(int b = -180; b < 180; b +=30.0)
    {
      double a = ((double)b)*Math.PI/180.0 + rotOffset - 0.06;
      // is mirrored!
      g2d.drawString(String.valueOf(b), (int)(Math.sin(a)*(maxDist+distStep+100)*0.1)-10, (int)(Math.cos(a)*(maxDist+distStep+100)*0.1)+5);
    }//end for

    g2d.setColor(new Color(backLight, backLight, colorLight));
    for(double a = 50; a <= maxDist/10.0; a +=(distStep/10.0))
    {
      double value = a*1e-2;// in m
      g2d.drawString(String.valueOf(value), -25, -(int)a-5);
      g2d.drawString(String.valueOf(value), -(int)a-20, 15);
    }//end for
    
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    g2d.rotate(Math.PI*0.5);
    g2d.scale(0.1, 0.1);

    // draw coordinate system
    g2d.setColor(new Color(backLight, backLight, backLight));
    g2d.setStroke(new BasicStroke(30.0f));
    new Arrow(-4000, 0, 4000, 0).draw(g2d);
    new Arrow(0, -4000, 0, 4000).draw(g2d);

    
    g2d.scale(10, 10);
    g2d.rotate(-Math.PI*0.5);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    // x-axis
    g2d.drawString("Y", -4000/10, -100/10);
    // y-axis
    g2d.drawString("X", 100/10, -4000/10);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    g2d.rotate(Math.PI*0.5);
    g2d.scale(0.1, 0.1);


    g2d.setColor(new Color(backLight, backLight, backLight));
    new Robot(0,0,0).draw(g2d);
  }//end draw

}//end class RadarDrawing
