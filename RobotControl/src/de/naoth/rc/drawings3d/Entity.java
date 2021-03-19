/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.drawings3d;

import com.sun.j3d.loaders.Loader;
import de.naoth.rc.Helper;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;
import java.util.zip.DataFormatException;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.Node;
import javax.media.j3d.Transform3D;
import org.jdesktop.j3d.loaders.vrml97.VrmlLoader;

public class Entity extends Drawable
{

  private static final String vrmlDir = "/de/naoth/rc/res/vrml/";
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
    InputStream is = getClass().getResourceAsStream(vrmlfile);
    BufferedReader reader = new BufferedReader(new InputStreamReader(is));
    
    try
    {
      loader.setBaseUrl(getClass().getResource(vrmlDir));
      com.sun.j3d.loaders.Scene vrmlScene = loader.load(reader);
      e = vrmlScene.getSceneGroup();
    } catch (Exception ex)
    {
      Helper.handleException("Could not load " + name + " with URL " + getClass().getResource(vrmlfile).toString(), ex);
    }
    return e;
  }

}
