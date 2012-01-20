/*
 * 
 */

package de.naoth.rc.dialogs.drawings;

import java.awt.Graphics2D;
import java.util.ArrayList;

/**
 *
 * @author Heinrich Mellmann
 */
public class DrawingCollection extends ArrayList<Drawable> implements Drawable
{

  public DrawingCollection(){}

  public void draw(Graphics2D g2d) {
    if(isEmpty()) return;

    synchronized(this)
    {
      for(Drawable drawing: this)
      {
        if(drawing != null)
          drawing.draw(g2d);
      }//end for
    }
  }//end draw
  
}//end DrawingCollection
