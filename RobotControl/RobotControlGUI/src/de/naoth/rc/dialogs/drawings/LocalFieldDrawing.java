/*
 * 
 */

package de.naoth.rc.dialogs.drawings;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class LocalFieldDrawing implements Drawable
{
  public void draw(Graphics2D g2d)
  {
    // draw gree pane
    g2d.setColor(new Color(0f, 1f, 0f, 0.5f));
    //g2d.fillRect(-3700, -2700, 7400, 5400);

    // draw grid
    g2d.setColor(new Color(0.7f, 0.7f, 0.7f));
    for(int a = -2000; a <= 2000; a +=500)
    {
      g2d.drawLine(a, -2000, a, 2000);
      g2d.drawLine(-2000, a, 2000, a);
    }//end for

    // draw coordinate system
    g2d.setStroke(new BasicStroke(30.0f));
    // x-axis
    g2d.drawString("X", 3000, 10);
    new Arrow(-3000, 0, 3000, 0).draw(g2d);
    // y-axis
    g2d.drawString("Y", 10, 3000);
    new Arrow(0, -3000, 0, 3000).draw(g2d);

    new Robot(0,0,0).draw(g2d);
  }//end draw
  
}//end class LocalFieldDrawing
