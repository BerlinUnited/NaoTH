/*
 * 
 */

package de.hu_berlin.informatik.ki.nao.manager;

import de.hu_berlin.informatik.ki.nao.dialogs.drawings.Drawable;
import de.hu_berlin.informatik.ki.nao.dialogs.drawings.DrawingsContainer;
import de.hu_berlin.informatik.ki.nao.server.Command;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
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

    //ArrayList<Drawable> drawingList = new ArrayList<Drawable>(messages.length);
    DrawingsContainer drawingList = new DrawingsContainer();

    String resultAsString = new String(result);
    if(resultAsString.trim().length() > 0)
    {
      String[] messages = resultAsString.split("\n");

      for(String str: messages)
      {
        String[] tokens = str.split(":");
        try{
          // the drawing-class should be in the same package as the interface Drawable
          String packageName = Drawable.class.getPackage().getName();
          // get the class of the drawing
          Class drawingClass = Class.forName(packageName+"."+tokens[0]);
          // get the constructor witch takes String[] as parameter
          Constructor constructor = drawingClass.getConstructor(tokens.getClass());
          // create new Drawing
          Object drawing = constructor.newInstance(new Object[] {tokens});

          if(drawing instanceof Drawable)
          {
            drawingList.add((Drawable)drawing);
          }
        }
        catch(ArrayIndexOutOfBoundsException e)
        {
          System.out.println("ERROR: DebugDrawingManager: " + str);
        }
        catch(ClassNotFoundException e)
        {
          System.out.println("ERROR: DebugDrawingManager: no such drawing: " + str);
        }
        catch(NoSuchMethodException e){}
        catch(InstantiationException e){}
        catch(IllegalAccessException e){}
        catch(InvocationTargetException e){}
      }//end for
    } // end if length > 0

    return drawingList;
  }//end convertByteArrayToType



  @Override
  public Command getCurrentCommand()
  {
    return new Command("debug_drawings");
  }

}//end class DebugDrawingManager
