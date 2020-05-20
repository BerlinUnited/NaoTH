package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import de.naoth.rc.drawings.Drawable;
import de.naoth.rc.drawings.DrawingFactory;
import de.naoth.rc.drawings.DrawingsContainer;
import de.naoth.rc.core.server.Command;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class DebugDrawingManagerImpl extends AbstractManagerPlugin<DrawingsContainer>
  implements DebugDrawingManager
{

  @Override
  public DrawingsContainer convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    DrawingsContainer drawingList = new DrawingsContainer();
    
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
      }
    } // end if length > 0

    return drawingList;
  }

  @Override
  public Command getCurrentCommand()
  {
    return new Command("Cognition:representation:get").addArg("DebugDrawings");
  }

}//end class DebugDrawingManager
