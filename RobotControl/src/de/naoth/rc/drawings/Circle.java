/*
 * 
 */
package de.naoth.rc.drawings;

import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class Circle implements Drawable
{

  private int x;
  private int y;
  private int radius;

  public Circle(int x, int y, int radius)
  {
    this.x = x;
    this.y = y;
    this.radius = radius;
  }

  public Circle(String[] args)
  {
    this.x = (int) (Double.parseDouble(args[1]));
    this.y = (int) (Double.parseDouble(args[2]));
    this.radius = (int) (Double.parseDouble(args[3]));
  }

  public void draw(Graphics2D g2d)
  {
    g2d.drawOval(x - radius, y - radius, 2 * radius, 2 * radius);
  }
}//end Circle

