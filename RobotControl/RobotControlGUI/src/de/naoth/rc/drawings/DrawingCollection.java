/*
 * 
 */

package de.naoth.rc.drawings;

import java.awt.Graphics2D;
import java.util.ArrayList;

/**
 *
 * @author Heinrich Mellmann
 */
public class DrawingCollection implements Drawable
{
  private final ArrayList<Drawable> drawables = new ArrayList<Drawable>();
  
  public void add(Drawable d) {
      this.drawables.add(d);
  }

  @Override
  public void draw(Graphics2D g2d) {
    for(Drawable drawing: drawables)
    {
      if(drawing != null)
        drawing.draw(g2d);
    }
  }//end draw
}//end DrawingCollection
