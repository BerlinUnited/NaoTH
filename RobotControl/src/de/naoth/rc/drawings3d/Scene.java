/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.drawings3d;

import de.naoth.rc.dataformats.JanusImage;
import java.awt.Color;

import java.util.Iterator;

import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.BranchGroup;
import org.jogamp.java3d.ColoringAttributes;
import org.jogamp.java3d.Group;
import org.jogamp.java3d.Node;
import org.jogamp.java3d.PolygonAttributes;

import org.jogamp.vecmath.Color3f;

public class Scene extends BranchGroup
{
  public Scene()
  {
    setCapability(Scene.ALLOW_DETACH);
    setCapability(Scene.ALLOW_CHILDREN_EXTEND);
    setCapability(Scene.ALLOW_CHILDREN_WRITE);
  }
  
  // find and return the camera with a certain id
  public Camera getCamera(String id) 
  {
    for ( Iterator<Node> e = getAllChildren(); e.hasNext();)
    {
      Object o = e.next();
      if (o instanceof Camera) {
        Camera c = (Camera)o;
        if(c.getId().equals(id)) {
            return c;
        }
      }
    }
    
    return null;
  }

  // set the image for the camre with the id
  public void addCameraImage(JanusImage image, String id)
  {
    Camera camera = getCamera(id);
    if(camera != null) {
        camera.setImage(image);
    }
  }

  public void print()
  {
    printNode(this, 0);
  }

  // list all nodes for debugging purposes
  public void printNode(Node n, int level)
  {
    String head = "";
    for( int i=0; i<level; i++)
    {
      head += "-";
    }
    System.out.println(head+n);
    if ( n instanceof Group )
    {
      Group g = (Group) n;
      Iterator<Node> child = g.getAllChildren();
      while( child.hasNext() )
      {
        printNode((Node) child.next(), level+1);
      }
    }
  }

  public static Appearance getAppearance(Color color)
  {
    Color3f c = new Color3f(color.getRGBColorComponents(null));
    Appearance app = new Appearance();
    app.setColoringAttributes(new ColoringAttributes(c, ColoringAttributes.SHADE_FLAT));
    app.setPolygonAttributes(new PolygonAttributes(
            PolygonAttributes.POLYGON_FILL,
            PolygonAttributes.CULL_NONE, 0));

    return app;
  }

}
