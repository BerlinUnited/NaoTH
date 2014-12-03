/*
 * 
 */

package de.naoth.rc.dialogs.drawings;

import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class Rotation implements Drawable
{
  private float angle = 0.0f;

  public Rotation(float angle)
  {
    this.angle = angle;
  }
  
  public Rotation(String[] args)
  {
    this.angle = Float.parseFloat(args[1]);
  }

  @Override
  public void draw(Graphics2D g2d)
  {
    g2d.rotate(angle);
  }//end draw
}//end class Rotation
