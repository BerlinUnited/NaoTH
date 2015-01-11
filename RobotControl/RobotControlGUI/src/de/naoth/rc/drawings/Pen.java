/*
 *
 */
package de.naoth.rc.drawings;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class Pen implements Drawable
{

  private float width;
  private Color color;

  public Pen(float width, Color color)
  {
    this.width = width;
    this.color = color;
  }

  public Pen(String[] args)
  {
    this.color = Colors.parseColor(args[1]);
    this.width = (int) (Float.parseFloat(args[2]));
  }

  public void draw(Graphics2D g2d)
  {
    g2d.setColor(color);
    g2d.setStroke(new BasicStroke(this.width));
  }//end draw
}//end class Pen

