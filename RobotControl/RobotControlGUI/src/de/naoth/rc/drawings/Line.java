/*
 * 
 */

package de.naoth.rc.drawings;

import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class Line implements Drawable
{
    private int x0;
    private int y0;
    private int x1;
    private int y1;

  public Line(int x0, int y0, int x1, int y1)
  {
    this.x0 = x0;
    this.y0 = y0;
    this.x1 = x1;
    this.y1 = y1;
  }

    

    public Line(String[] args)
    {
        this.x0 = (int)(Double.parseDouble(args[1]));
        this.y0 = (int)(Double.parseDouble(args[2]));
        this.x1 = (int)(Double.parseDouble(args[3]));
        this.y1 = (int)(Double.parseDouble(args[4]));
    }

    public void draw(Graphics2D g2d)
    {
        g2d.drawLine(x1, y1, x0, y0);
    }
}//end Line
