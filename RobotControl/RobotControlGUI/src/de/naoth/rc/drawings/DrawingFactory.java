
package de.naoth.rc.drawings;

import java.lang.reflect.Constructor;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;


public class DrawingFactory 
{
    private static DrawingFactory instance = new DrawingFactory();
    private Map<String, Constructor> drawableRegistry = new HashMap<>();
    
    private DrawingFactory() {}
    
    private Drawable createDrawingFromArray(String[] tokens) 
    {
      try {

        Constructor constructor = drawableRegistry.get(tokens[0]);

        if (constructor == null)
        {
          /*
          // experiment for acceleration
          String name = tokens[0];
          if(name.length() == 1) {
              switch(name.charAt(0)) {
                  case 'B': name = "FillBox"; break;
                  case 'p': name = "Pen"; break;
              }
          }
          */

          // the drawing-class should be in the same package as the interface Drawable
          String packageName = Drawable.class.getPackage().getName();
          // get the class of the drawing
          Class drawingClass = Class.forName(packageName+"."+tokens[0]);
          // get the constructor witch takes String[] as parameter
          constructor = drawingClass.getConstructor(tokens.getClass());
          // store the constructor under the original token
          drawableRegistry.put(tokens[0], constructor);
        }

        // create new Drawing
        Object drawing = constructor.newInstance(new Object[] {tokens});

        if(drawing instanceof Drawable)
        {
          return (Drawable)drawing;
        }
      }
      catch(Exception ex) {
          Logger.getLogger(DrawingFactory.class.getName()).log(Level.SEVERE, 
            "ERROR: Could not create a drawing: " + String.join(":", tokens), ex);
      }

      return null;
    }
    
    
    public static Drawable createDrawing(String[] tokens) {
        return instance.createDrawingFromArray(tokens);
    }
}

