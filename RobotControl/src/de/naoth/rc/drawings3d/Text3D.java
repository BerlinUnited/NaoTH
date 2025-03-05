/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.drawings3d;

import java.awt.Font;
import java.util.zip.DataFormatException;
import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.Font3D;
import org.jogamp.java3d.FontExtrusion;
import org.jogamp.java3d.OrientedShape3D;
import org.jogamp.java3d.Shape3D;
import org.jogamp.vecmath.Point3f;

public class Text3D extends Drawable
{

  private static Font3D f3d = new Font3D(new Font("Font3D", Font.PLAIN, 1), new FontExtrusion());

  public Text3D(String[] tokens) throws DataFormatException
  {
    Appearance app = parseAppearance(tokens[1]);
    String text = new String(tokens[2]);
    for (int i=3; i<tokens.length-3;i++){
      text += ' ';
      text += tokens[i];
    }
    float[] p = parseFloatArray(tokens, tokens.length-3, tokens.length);
    org.jogamp.java3d.Text3D t = new org.jogamp.java3d.Text3D(f3d, text);
    Shape3D s = new OrientedShape3D(t, app, OrientedShape3D.ROTATE_ABOUT_POINT, new Point3f(p), true, 0.5);
    addChild(s);
  }
}
