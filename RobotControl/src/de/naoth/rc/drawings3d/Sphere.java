/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.drawings3d;

import java.util.zip.DataFormatException;
import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.Transform3D;
import org.jogamp.vecmath.Vector3f;

public class Sphere extends Drawable
{

  public Sphere(String[] tokens) throws DataFormatException
  {
    Appearance app = parseAppearance(tokens[1]);
    float radius = Float.valueOf(tokens[2]) * 0.001f;
    float[] p = parseFloatArray(tokens, 3, 6);
    Transform3D t = new Transform3D();
    Vector3f vec = new Vector3f(p);
    t.setTranslation(vec);
    setTransform(t);
    org.jogamp.java3d.utils.geometry.Sphere s = new org.jogamp.java3d.utils.geometry.Sphere(radius, app);
    addChild(s);
  }
}
