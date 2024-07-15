/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.drawings3d;

import java.util.zip.DataFormatException;
import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.LineArray;
import org.jogamp.java3d.Shape3D;
import org.jogamp.vecmath.Point3f;

public class Line extends Drawable
{

  public Line(String[] tokens) throws DataFormatException
  {
    Appearance app = parseAppearance(tokens[1]);
    float[] a = parseFloatArray(tokens, 2, 8);
    LineArray la = new LineArray(2, LineArray.COORDINATES);
    la.setCoordinates(0, a);
    Shape3D line = new Shape3D(la, app);
    addChild(line);
  }

  public Line(Point3f[] points, Appearance app)
  {
    LineArray la = new LineArray(points.length, LineArray.COORDINATES);
    la.setCoordinates(0, points);
    Shape3D line = new Shape3D(la, app);
    addChild(line);
  }
}
