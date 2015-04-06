/**
 * @author <a href="xu:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import de.naoth.rc.drawings3d.Drawable;
import de.naoth.rc.drawings3d.Scene;
import de.naoth.rc.server.Command;
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

  public ThreeDimensionSceneManagerImpl()
  {
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
        try
        {
          String[] tokens = str.split(" ");
          // get the class of the drawing
          Class drawingClass = Class.forName(drawablePackageName + "." + tokens[0]);
          // get the constructor which takes String[] as parameter
          Constructor constructor = drawingClass.getConstructor(tokens.getClass());
          // create new Drawing
          Object drawing = constructor.newInstance(new Object[]
                  {
                    tokens
                  });
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
          Logger.getLogger(ThreeDimensionSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
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
  public Command getCurrentCommand()
  {
    return new Command("Cognition:representation:getbinary").addArg("DebugDrawings3D");
  }
}
