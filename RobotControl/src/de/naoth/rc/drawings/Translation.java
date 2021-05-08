/*
 *
 */

package de.naoth.rc.drawings;

import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class Translation implements Drawable
{
  private float x = 0.0f;
  private float y = 0.0f;

  public Translation(float x, float y)
  {
    this.x = x;
    this.y = y;
  }

  public Translation(String[] args)
  {
    this.x = Float.parseFloat(args[1]);
    this.y = Float.parseFloat(args[2]);
  }

  @Override
  public void draw(Graphics2D g2d)
  {
    g2d.translate(x,y);
  }
}//end class Rotation
