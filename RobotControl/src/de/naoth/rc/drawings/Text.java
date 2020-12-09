/*
 *
 */

package de.naoth.rc.drawings;

import java.awt.Font;
import java.awt.FontMetrics;
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
  private final double rotation;
  private final String text;
  private static int defaultSize = 250;

  private Font myFont;
  
  public Text(String[] args)
  {
    this((int)(Double.parseDouble(args[1])),
         (int)(Double.parseDouble(args[2])),
         args.length == 4 ? defaultSize : (int)(Double.parseDouble(args[3])*defaultSize),
         args.length == 4 ? args[3] : args[4]);
  }

  public Text(int x, int y, String text) {
    this(x, y, defaultSize, text);
  }

  public Text(int x, int y, int s, String text) {
    this(x, y, 0.0, text, new Font ("Courier New", Font.PLAIN, s));
  }
    
  public Text(int x, int y, double rotation, String text, Font font) {
    this.x = x;
    this.y = y;
    this.rotation = rotation;
    this.text = text;
    this.myFont = font;
  }

  @Override
  public void draw(Graphics2D g2d)
  {
    Font oldFont = g2d.getFont();
    g2d.setFont (myFont);
    
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    g2d.translate(x, -y);
    g2d.rotate(-this.rotation);
    
    FontMetrics metrics = g2d.getFontMetrics(this.myFont);
    // Determine the X coordinate for the text
    int x0 = - metrics.stringWidth(text) / 2;
    // Determine the Y coordinate for the text (note we add the ascent, as in java 2d 0 is top of the screen)
    int y0 = - metrics.getHeight() / 2 + metrics.getAscent();
    g2d.drawString(text, x0, y0);
    
    g2d.rotate(this.rotation);
    g2d.translate(-x, y);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    
    // IMPORTANT: reset the font
    g2d.setFont(oldFont);
  }
}//end Circle

