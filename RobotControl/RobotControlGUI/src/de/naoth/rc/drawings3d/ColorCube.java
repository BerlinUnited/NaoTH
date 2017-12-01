/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

package de.naoth.rc.drawings3d;

import java.util.zip.DataFormatException;
import javax.media.j3d.Transform3D;
import javax.vecmath.Vector3f;

public class ColorCube extends Drawable {

  public ColorCube(String[] tokens) throws DataFormatException
  {
    float[] dim = parseFloatArray(tokens, 1, 2);
    Transform3D t = parseTransform3D(tokens, 2);

    setTransform(t);
    com.sun.j3d.utils.geometry.ColorCube b = new com.sun.j3d.utils.geometry.ColorCube(dim[0]);
    addChild(b);
  }

  public ColorCube(Vector3f p, float size)
  {
    Transform3D t = new Transform3D();
    t.setTranslation(p);

    setTransform(t);
    com.sun.j3d.utils.geometry.ColorCube b = new com.sun.j3d.utils.geometry.ColorCube(size);
    addChild(b);
  }
}