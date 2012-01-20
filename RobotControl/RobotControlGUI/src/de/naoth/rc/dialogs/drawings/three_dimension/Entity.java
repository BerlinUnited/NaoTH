/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.dialogs.drawings.three_dimension;

import com.sun.j3d.loaders.Loader;
import de.naoth.rc.Helper;
import java.util.HashMap;
import java.util.Map;
import java.util.zip.DataFormatException;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.Node;
import javax.media.j3d.Transform3D;
import org.jdesktop.j3d.loaders.vrml97.VrmlLoader;

public class Entity extends Drawable
{

  private static final String vrmlDir = "src/de/hu_berlin/informatik/ki/nao/res/vrml/";
  private static final Loader loader = new VrmlLoader(Loader.LOAD_ALL);
  private static Map<String, BranchGroup> loadedEnities = new HashMap();

  public Entity(String[] token) throws DataFormatException
  {
    Transform3D t = parseTransform3D(token, 2);
    setTransform(t);
    create(token[1]);
  }

  public Entity(String name)
  {
    create(name);
  }

  public Entity(String name, Transform3D t)
  {
    setTransform(t);
    create(name);
  }

  private void create(String name)
  {
     synchronized (loader)
    {
      BranchGroup e = loadedEnities.get(name);
      if (e == null)
      {
        e = loadFromVRML(name);
        loadedEnities.put(name, e);
      }

      Node n = e.cloneTree();
      addChild(n);
    }
  }

  private BranchGroup loadFromVRML(String name)
  {
    BranchGroup e = null;
    String vrmlfile = vrmlDir + name + ".wrl";
      try
      {
        com.sun.j3d.loaders.Scene vrmlScene = loader.load(vrmlfile);
        e = vrmlScene.getSceneGroup();
      } catch (Exception ex)
      {
        Helper.handleException(name, ex);
      }
    return e;
  }

}
