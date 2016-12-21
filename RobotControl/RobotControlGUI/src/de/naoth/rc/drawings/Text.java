/*
 *
 */

package de.naoth.rc.drawings;

import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;

/**
 *
 * @author Heinrich Mellmann
 */
public class Text implements Drawable
{
  private final int x;
  private final int y;
  private final String text;

  private final Font myFont = new Font ("Courier New", Font.PLAIN, 250);
  
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

  @Override
  public void draw(Graphics2D g2d)
  {
    Font oldFont = g2d.getFont();
    g2d.setFont (myFont);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    g2d.drawString(text, x, -y);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    
    // IMPORTANT: reset the font
    g2d.setFont(oldFont);
  }
}//end Circle

