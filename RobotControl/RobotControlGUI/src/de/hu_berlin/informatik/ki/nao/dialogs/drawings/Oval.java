/*
 * 
 */

package de.hu_berlin.informatik.ki.nao.dialogs.drawings;

import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class Oval implements Drawable
{
  protected int x;
  protected int y;
  protected int radiusX;
  protected int radiusY;

  public Oval(String[] args)
  {
      this.x = (int)(Double.parseDouble(args[1]));
      this.y = (int)(Double.parseDouble(args[2]));
      this.radiusX = (int)(Double.parseDouble(args[3]));
      this.radiusY = (int)(Double.parseDouble(args[4]));
  }

  public void draw(Graphics2D g2d)
  {
      g2d.drawOval(x - radiusX, y - radiusY, 2*radiusX, 2*radiusY);
  }//end draw
}//end Oval
