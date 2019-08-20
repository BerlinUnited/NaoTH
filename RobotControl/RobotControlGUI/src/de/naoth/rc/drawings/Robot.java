/*
 * 
 */

package de.naoth.rc.drawings;

import de.naoth.rc.math.Vector2D;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.GradientPaint;
import java.awt.Graphics2D;
import java.awt.Polygon;
import java.awt.RadialGradientPaint;
import java.awt.geom.Arc2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class Robot implements Drawable
{
  protected int x;
  protected int y;
  protected double rotation;

  protected double headRotation = 0.0;
  private boolean drawHeadRotation = false;
  protected Color color = new Color(234, 62, 50, 255);

  private final Nao_Simple nao = new Nao_Simple();

  public Robot(double x, double y, double rotation, double headRotation)
  {
    this(x,y,rotation);
    this.headRotation = headRotation;
    this.drawHeadRotation = true;
  }
  
  public Robot(double x, double y, double rotation)
  {
    this.x = (int)(x);
    this.y = (int)(y);
    this.rotation = rotation;
  }
  
  public Robot(String[] args)
  {
    this.x = (int)(Double.parseDouble(args[1]));
    this.y = (int)(Double.parseDouble(args[2]));
    this.rotation = Double.parseDouble(args[3]);

    if(args.length > 4) {
      this.headRotation = Double.parseDouble(args[4]);
      this.drawHeadRotation = true;
    }

    if(args.length > 5) {
      this.color = Colors.parseColor(args[5]);
    }
  }

  @Override
  public void draw(Graphics2D g2d)
  {
    g2d.translate(x, y);
    g2d.rotate(rotation);

    double factor = 1.5;
    double centerOffsetX = Nao_Simple.getOrigX() + Nao_Simple.getOrigWidth()/2;
    double centerOffsetY = Nao_Simple.getOrigY() + Nao_Simple.getOrigHeight()/2;


    g2d.scale(factor,factor);
    g2d.translate(-centerOffsetX, -centerOffsetY);
    
    nao.setHeadYaw(this.headRotation);
    Color currentColor = g2d.getColor();
    nao.setColor((currentColor==null)?this.color:currentColor);
    nao.paint(g2d);
    
    g2d.translate(centerOffsetX, centerOffsetY);
    g2d.scale(1.0/factor,1.0/factor);

    if(drawHeadRotation) {
        drawAngleInterval(g2d, headRotation - 30.0/180.0*Math.PI, headRotation + 30.0/180.0*Math.PI, Color.BLUE);
    }
    
    /* Debug (to adjust the size of the robot) */
    /*
    int ShoulderOffsetY = 98;
    int radius = 3;
    g2d.setColor(Color.blue);
    g2d.drawOval(0-radius, 0-radius, 2*radius, 2*radius);
    g2d.drawOval(0-radius, 0+ShoulderOffsetY-radius, 2*radius, 2*radius);
    g2d.drawOval(0-radius, 0-ShoulderOffsetY-radius, 2*radius, 2*radius);
    */
    
    g2d.rotate(-rotation);
    g2d.translate(-x, -y);
  }//end draw
  
  private void drawAngleInterval(Graphics2D g2d, double min, double max, Color color)
  {
    double distance = 600.0;
    Color start = new Color(color.getRed(),color.getGreen(),color.getBlue(),128);
    Color end = new Color(color.getRed(),color.getGreen(),color.getBlue(),0);

    Vector2D a = new Vector2D(distance,0).rotate(min);
    Vector2D b = new Vector2D(distance,0).rotate(max);
    Vector2D p = (a.add(b).multiply(0.5)).normalize().multiply(70);
    a = a.add(p);
    b = b.add(p);
    
    
    //double angleDiff = a.angleTo(b);
    
    
    RadialGradientPaint blacktowhite = new RadialGradientPaint((float)p.x, (float)p.y, (float)distance, 
                            new float[]{0f, 1f},
                            new Color[]{Color.black, end});
    
    RadialGradientPaint fildOfView = new RadialGradientPaint((float)p.x, (float)p.y, (float)distance, 
                            new float[]{0f, 1f},
                            new Color[]{start, end});
    
    g2d.setStroke(new BasicStroke(1f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
    g2d.setPaint(fildOfView);
    g2d.fill(new Arc2D.Double(-distance+p.x, -distance+p.y, distance*2, distance*2, -Math.toDegrees(min), -Math.toDegrees(max -min), Arc2D.PIE));

    g2d.setPaint(blacktowhite);
    g2d.setStroke(new BasicStroke(10f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
    g2d.drawLine((int)p.x, (int)p.y, (int)a.x, (int)a.y);
    g2d.drawLine((int)p.x, (int)p.y, (int)b.x, (int)b.y);
  }
  
}//end class Robot
