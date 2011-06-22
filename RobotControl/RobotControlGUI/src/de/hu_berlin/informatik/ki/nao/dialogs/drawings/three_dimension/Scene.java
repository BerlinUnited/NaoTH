/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.hu_berlin.informatik.ki.nao.dialogs.drawings.three_dimension;

import de.hu_berlin.informatik.ki.nao.dataformats.JanusImage;
import java.awt.Color;
import java.util.Enumeration;
import javax.media.j3d.Appearance;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.ColoringAttributes;
import javax.media.j3d.Group;
import javax.media.j3d.Node;
import javax.media.j3d.PolygonAttributes;
import javax.vecmath.Color3f;

public class Scene extends BranchGroup
{

  public Scene()
  {
    setCapability(Scene.ALLOW_DETACH);
    setCapability(Scene.ALLOW_CHILDREN_EXTEND);
    setCapability(Scene.ALLOW_CHILDREN_WRITE);
  }

  public void addCameraImage(JanusImage image)
  {
    for ( Enumeration e = getAllChildren(); e.hasMoreElements();)
    {
      Object o = e.nextElement();
      if (Camera.class.isInstance(o))
      {
        Camera c = (Camera)o;
        c.addImage(image);
        return;
      }
    }
  }

  public void print()
  {
    printNode(this, 0);
  }

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
      Enumeration child = g.getAllChildren();
      while( child.hasMoreElements() )
      {
        printNode((Node) child.nextElement(), level+1);
      }
    }
  }

  public static Appearance getAppearance(Color color)
  {
    Color3f c = new Color3f(color);
    Appearance app = new Appearance();
    app.setColoringAttributes(new ColoringAttributes(c, ColoringAttributes.SHADE_FLAT));
    app.setPolygonAttributes(new PolygonAttributes(
            PolygonAttributes.POLYGON_FILL,
            PolygonAttributes.CULL_NONE, 0));

    return app;
  }

}
