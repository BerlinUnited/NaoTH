/*
 * 
 */

package de.naoth.rc.dialogs.drawings;

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

  protected double rotation = 0.0;

  public Oval(String[] args)
  {
      this.x = (int)(Double.parseDouble(args[1]));
      this.y = (int)(Double.parseDouble(args[2]));
      this.radiusX = (int)(Double.parseDouble(args[3]));
      this.radiusY = (int)(Double.parseDouble(args[4]));

      if(args.length > 5)
        this.rotation = Double.parseDouble(args[5]);
  }

  public Oval(int x, int y, int radiusX, int radiusY, double rotation) {
    this.x = x;
    this.y = y;
    this.radiusX = radiusX;
    this.radiusY = radiusY;
    this.rotation = rotation;
  }

  public Oval(int x, int y, int radiusX, int radiusY) {
    this.x = x;
    this.y = y;
    this.radiusX = radiusX;
    this.radiusY = radiusY;
  }

  public void draw(Graphics2D g2d)
  {
    g2d.translate(x, y);
    g2d.rotate(rotation);
    
    g2d.drawOval(-radiusX, -radiusY, 2*radiusX, 2*radiusY);

    g2d.rotate(-rotation);
    g2d.translate(-x, -y);
  }//end draw
}//end Oval
