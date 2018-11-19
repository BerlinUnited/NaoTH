/*
 * 
 */

package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import de.naoth.rc.drawings.Drawable;
import de.naoth.rc.drawings.DrawingFactory;
import de.naoth.rc.drawings.DrawingsContainer;
import de.naoth.rc.server.Command;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class DebugDrawingManagerImpl extends AbstractManagerPlugin<DrawingsContainer>
  implements DebugDrawingManager
{

  public DebugDrawingManagerImpl()
  {
  }


  @Override
  public DrawingsContainer convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    DrawingsContainer drawingList = new DrawingsContainer();
    
    double start = System.currentTimeMillis();
    String resultAsString = new String(result);
    if(resultAsString.trim().length() > 0)
    {
      String[] messages = resultAsString.split("\n");

      for(String message: messages)
      {
        Drawable drawing = DrawingFactory.createDrawing(message.split(":"));
        if(drawing != null) {
          drawingList.add((Drawable)drawing);
        }
      }//end for
    } // end if length > 0
    
    System.out.println(System.currentTimeMillis() - start);
    
    return drawingList;
  }//end convertByteArrayToType

  @Override
  public Command getCurrentCommand()
  {
    return new Command("Cognition:representation:get").addArg("DebugDrawings");
  }

}//end class DebugDrawingManager
