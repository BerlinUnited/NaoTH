/*
 *
 */

package de.naoth.rc.dialogs.drawings;

import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;

/**
 *
 * @author Heinrich Mellmann
 */
public class Text implements Drawable
{
  private int x;
  private int y;
  private String text;

  public Text(String[] args)
  {
    this.x = (int)(Double.parseDouble(args[1]));
    this.y = (int)(Double.parseDouble(args[2]));
    this.text = args[3];
  }

  public Text(int x, int y, String text) {
    this.x = x;
    this.y = y;
    this.text = text;
  }

  public void draw(Graphics2D g2d)
  {
    g2d.scale(10, 10);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    g2d.drawString(text, x/10, -y/10);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    g2d.scale(0.1, 0.1);
  }
}//end Circle

