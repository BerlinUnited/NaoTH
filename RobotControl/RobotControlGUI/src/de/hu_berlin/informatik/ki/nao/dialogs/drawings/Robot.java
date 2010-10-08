/*
 * 
 */

package de.hu_berlin.informatik.ki.nao.dialogs.drawings;

import de.hu_berlin.informatik.ki.nao.dialogs.Tools.Colors;
import java.awt.Color;
import java.awt.Graphics2D;

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
  protected Color color = new Color(234, 62, 50, 255);

  private Nao_Simple nao = new Nao_Simple();

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

    if(args.length > 4) 
      this.headRotation = Double.parseDouble(args[4]);

    if(args.length > 5)
      this.color = Colors.parseColor(args[5]);
  }

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
}//end class Robot
