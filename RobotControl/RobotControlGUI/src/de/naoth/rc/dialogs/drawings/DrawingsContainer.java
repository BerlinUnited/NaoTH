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
  private final HashMap<Class, DrawingCollection> drawingMap = new HashMap<Class, DrawingCollection>();
  private final HashMap<String, Canvas> drawingMapId = new HashMap<String, Canvas>();
  
  private DrawingCollection currentCollection = null;
  private Canvas currentCanvas = null;

  public DrawingsContainer()
  {
    currentCollection = new DrawingCollection();
    drawingMap.put(currentCollection.getClass(), currentCollection);
  }

  public DrawingCollection get(Class cl)
  {
    return drawingMap.get(cl);
  }
  
  public Canvas get(String id)
  {
    return drawingMapId.get(id);
  }

  public void add(Drawable drawing)
  {
    if(drawing instanceof Canvas)
    {
      Canvas canvas = (Canvas)drawing;
      if(drawingMapId.get(canvas.id) == null) {
        drawingMapId.put(canvas.id, canvas);
      }

      currentCanvas = drawingMapId.get(canvas.id);
      currentCollection = null;
      return;
    }//end if
      
    if(drawing instanceof DrawingCollection)
    {
      if(drawingMap.get(drawing.getClass()) == null)
        drawingMap.put(drawing.getClass(), (DrawingCollection)drawing);

      currentCollection = drawingMap.get(drawing.getClass());
      return;
    }//end if

    if(currentCollection != null) {
      currentCollection.add(drawing);
    } else if(currentCanvas != null){
      currentCanvas.add(drawing);
    }
  }//end add
}//end class DrawingsContainer
