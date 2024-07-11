/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

package de.naoth.rc.drawings3d;

import java.awt.Color;
import java.util.zip.DataFormatException;
import org.jogamp.java3d.Appearance;

import org.jogamp.java3d.ColoringAttributes;
import org.jogamp.java3d.PolygonAttributes;
import org.jogamp.java3d.RenderingAttributes;
import org.jogamp.java3d.Transform3D;
import org.jogamp.vecmath.Color3f;
import org.jogamp.vecmath.Vector3f;

public class Box extends Drawable {

  public Box(String[] tokens) throws DataFormatException
  {
    Appearance app = parseAppearance(tokens[1]);
    float[] dim = parseFloatArray(tokens, 2, 5);

    Transform3D t = parseTransform3D(tokens, 5);

    setTransform(t);
    org.jogamp.java3d.utils.geometry.Box b = new org.jogamp.java3d.utils.geometry.Box(dim[0], dim[1], dim[2], app);
    addChild(b);
  }

  public Box(Vector3f p, Vector3f dim, Color color)
  {
    Color3f c = new Color3f(color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f);
    Appearance app = new Appearance();
    app.setColoringAttributes(new ColoringAttributes(c, ColoringAttributes.SHADE_FLAT));
    app.setPolygonAttributes(new PolygonAttributes(
          PolygonAttributes.POLYGON_LINE,
          PolygonAttributes.CULL_NONE, 0));

    Transform3D t = new Transform3D();
    t.setTranslation(p);

    setTransform(t);
    org.jogamp.java3d.utils.geometry.Box b = new org.jogamp.java3d.utils.geometry.Box(dim.x, dim.y, dim.z, app);
    addChild(b);
  }
}