/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

package de.naoth.rc.drawings3d;

import java.awt.Color;
import java.util.zip.DataFormatException;
import javax.media.j3d.Appearance;
import javax.media.j3d.ColoringAttributes;
import javax.media.j3d.PolygonAttributes;
import javax.media.j3d.RenderingAttributes;
import javax.media.j3d.Transform3D;
import javax.vecmath.Color3f;
import javax.vecmath.Vector3f;

public class Box extends Drawable {

  public Box(String[] tokens) throws DataFormatException
  {
    Appearance app = parseAppearance(tokens[1]);
    float[] dim = parseFloatArray(tokens, 2, 5);

    Transform3D t = parseTransform3D(tokens, 5);

    setTransform(t);
    com.sun.j3d.utils.geometry.Box b = new com.sun.j3d.utils.geometry.Box(dim[0], dim[1], dim[2], app);
    addChild(b);
  }

  public Box(Vector3f p, Vector3f dim, Color color)
  {
    Color3f c = new Color3f(color);
    Appearance app = new Appearance();
    app.setColoringAttributes(new ColoringAttributes(c, ColoringAttributes.SHADE_FLAT));
    app.setPolygonAttributes(new PolygonAttributes(
          PolygonAttributes.POLYGON_LINE,
          PolygonAttributes.CULL_NONE, 0));

    Transform3D t = new Transform3D();
    t.setTranslation(p);

    setTransform(t);
    com.sun.j3d.utils.geometry.Box b = new com.sun.j3d.utils.geometry.Box(dim.x, dim.y, dim.z, app);
    addChild(b);
  }
}