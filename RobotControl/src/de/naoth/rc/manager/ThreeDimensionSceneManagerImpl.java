/**
 * @author <a href="xu:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import de.naoth.rc.drawings3d.Drawable;
import de.naoth.rc.drawings3d.Scene;
import de.naoth.rc.core.server.Command;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

@PluginImplementation
public class ThreeDimensionSceneManagerImpl extends AbstractManagerPlugin<Scene>
        implements ThreeDimensionSceneManager
{

  private final String drawablePackageName = Drawable.class.getPackage().getName();
  
  //private final TreeSet<String> warningsForEntities = new TreeSet<String>();
  
  private Command command = null;
  
  public ThreeDimensionSceneManagerImpl()
  {
      setModuleOwner("Cognition");
      
      // TODO:
      Logger.getLogger(ThreeDimensionSceneManagerImpl.class.getName()).log(Level.WARNING, 
          "[ThreeDimensionSceneManagerImpl] Fix missing Lhand and RHand objects in the 3D viever");
  }

  @Override
  public Scene convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    Scene scene = new Scene();
    String[] messages = (new String(result)).split("\n");
    for (String str : messages)
    {
      if (!str.isEmpty())
      {
        String[] tokens = str.split(" ");
        
        try
        {
            
          // HACK: Lhand and RHand objects are missing, ignore them for now
          if(tokens.length > 1 && ("LHand".equals(tokens[1]) || "RHand".equals(tokens[1]))) {
              continue;
          }
            
          // get the class of the drawing
          Class drawingClass = Class.forName(drawablePackageName + "." + tokens[0]);
          
          // get the constructor which takes String[] as parameter
          Constructor constructor = drawingClass.getConstructor(tokens.getClass());
          // create new Drawing
          Object drawing = constructor.newInstance(new Object[]{tokens});
          
          if (drawing instanceof Drawable)
          {
            scene.addChild((Drawable) drawing);
          }
        } catch (InstantiationException ex)
        {
          Logger.getLogger(ThreeDimensionSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex)
        {
          Logger.getLogger(ThreeDimensionSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
        } catch (InvocationTargetException ex)
        {
          String msg = "Could not create entity " + str;
          Logger.getLogger(ThreeDimensionSceneManagerImpl.class.getName()).log(Level.SEVERE, msg, ex);
        } catch (NoSuchMethodException ex)
        {
          Logger.getLogger(ThreeDimensionSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
        } catch (SecurityException ex)
        {
          Logger.getLogger(ThreeDimensionSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
        } catch (ClassNotFoundException ex)
        {
          Logger.getLogger(ThreeDimensionSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
        }
      }
    }
    return scene;
  }

  @Override
  final public void setModuleOwner(String name)
  {
      if(name != null && name.length() > 0) {
          command = new Command(name + ":representation:get").addArg("DebugDrawings3D");
      }
  }
  
  @Override
  public Command getCurrentCommand()
  {
    return command;
  }
}
