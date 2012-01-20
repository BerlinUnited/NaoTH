/*
 * 
 */

package de.naoth.rc.dialogs.drawings;

import java.util.HashMap;

/**
 *
 * @author Heinrich Mellmann
 */
public class DrawingsContainer
{
  HashMap<Class, DrawingCollection> drawingMap = new HashMap<Class, DrawingCollection>();
  DrawingCollection currentCollection = null;

  public DrawingsContainer()
  {
    currentCollection = new DrawingCollection();
    drawingMap.put(currentCollection.getClass(), currentCollection);
  }

  public DrawingCollection get(Class cl)
  {
    return drawingMap.get(cl);
  }

  public void add(Drawable drawing)
  {
    if(drawing instanceof DrawingCollection)
    {
      if(drawingMap.get(drawing.getClass()) == null)
        drawingMap.put(drawing.getClass(), (DrawingCollection)drawing);

      currentCollection = drawingMap.get(drawing.getClass());
      return;
    }//end if

    if(currentCollection != null)
    {
      currentCollection.add(drawing);
    }
  }//end add
}//end class DrawingsContainer
