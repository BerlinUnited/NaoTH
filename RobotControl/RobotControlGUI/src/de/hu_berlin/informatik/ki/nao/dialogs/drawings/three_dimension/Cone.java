/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.hu_berlin.informatik.ki.nao.dialogs.drawings.three_dimension;

import java.util.zip.DataFormatException;
import javax.media.j3d.Appearance;
import javax.media.j3d.Transform3D;

public class Cone extends Drawable {
  public Cone(String[] tokens) throws DataFormatException
  {
    Appearance app = parseAppearance(tokens[1]);
    float[] dim = parseFloatArray(tokens, 2, 4);
    Transform3D t = parseTransform3D(tokens, 4);
    setTransform(t);
    com.sun.j3d.utils.geometry.Cone c = new com.sun.j3d.utils.geometry.Cone(dim[0], dim[1], app);
    addChild(c);
  }
}
