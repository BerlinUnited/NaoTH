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
public class Canvas extends ArrayList<Drawable> implements Drawable
{
  public final String id;
  public Canvas(String[] args)
  {
      this.id = args[1];
  }

  @Override
  public void draw(Graphics2D g2d) {
    if(isEmpty()) return;

    synchronized(this)
    {
      for(Drawable drawing: this)
      {
        if(drawing != null) {
          drawing.draw(g2d);
        }
      }//end for
    }
  }//end draw
  
}//end DrawingCollection
