/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.hu_berlin.informatik.ki.nao.dialogs.drawings.three_dimension;

import java.awt.Font;
import java.util.zip.DataFormatException;
import javax.media.j3d.Appearance;
import javax.media.j3d.Font3D;
import javax.media.j3d.FontExtrusion;
import javax.media.j3d.OrientedShape3D;
import javax.media.j3d.Shape3D;
import javax.vecmath.Point3f;

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
    javax.media.j3d.Text3D t = new javax.media.j3d.Text3D(f3d, text);
    Shape3D s = new OrientedShape3D(t, app, OrientedShape3D.ROTATE_ABOUT_POINT, new Point3f(p), true, 0.5);
    addChild(s);
  }
}
