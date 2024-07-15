/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.drawings3d;

import java.util.zip.DataFormatException;
import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.Transform3D;

public class Cone extends Drawable {
  public Cone(String[] tokens) throws DataFormatException
  {
    Appearance app = parseAppearance(tokens[1]);
    float[] dim = parseFloatArray(tokens, 2, 4);
    Transform3D t = parseTransform3D(tokens, 4);
    setTransform(t);
    org.jogamp.java3d.utils.geometry.Cone c = new org.jogamp.java3d.utils.geometry.Cone(dim[0], dim[1], app);
    addChild(c);
  }
}
