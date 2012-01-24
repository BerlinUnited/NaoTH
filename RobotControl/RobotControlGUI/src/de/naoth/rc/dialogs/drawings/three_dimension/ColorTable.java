/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.dialogs.drawings.three_dimension;

import de.naoth.rc.dialogs.Tools.Colors;
import java.awt.Color;
import javax.media.j3d.Appearance;
import javax.vecmath.Point3f;
import javax.vecmath.Vector3f;

public class ColorTable extends Drawable {

  private de.naoth.rc.dialogs.Tools.ColorTable colorTable = new de.naoth.rc.dialogs.Tools.ColorTable();
  
  public ColorTable(byte[] result) throws Exception
  {
    colorTable.createFromCharArray((new String(result)).toCharArray());
    Vector3f boxDim = new Vector3f(0.01f, 0.01f, 0.01f);
    int[] size = colorTable.getDimension();
    int skip = 1;
    float distance = 0.02f;
    float[] offset = new float[3];
    for(int i=0; i<offset.length; i++){
      offset[i] = -size[i] * distance * 0.5f;
    }

    for (int x = 0; x < size[0]; x+=skip)
    {
      for (int y = 0; y < size[1]; y+=skip)
      {
        for (int z = 0; z < size[2]; z+=skip)
        {
          Colors.ColorClass c = colorTable.getColor((char)x, (char)y, (char)z);
          if (Colors.ColorClass.none != c)
          {
            Color color = Colors.GetColor(c);
            addChild(new Box(new Vector3f(x * distance + offset[0], y * distance+offset[1], z * distance+offset[2]), boxDim, color));
          }
        }
      }
    }

    // draw axes around with line
    Appearance appX = parseAppearance("FF0000");
    Point3f[] x = new Point3f[2];
    x[0] = new Point3f(offset[0], offset[1], offset[2]);
    x[1] = new Point3f(-offset[0], offset[1], offset[2]);
    addChild(new Line(x, appX));
    Appearance appY = parseAppearance("00FF00");
    Point3f[] y = new Point3f[2];
    y[0] = x[0];
    y[1] = new Point3f(offset[0], -offset[1], offset[2]);
    addChild(new Line(y, appY));
    Appearance appZ = parseAppearance("0000FF");
    Point3f[] z = new Point3f[2];
    z[0] = x[0];
    z[1] = new Point3f(offset[0], offset[1], -offset[2]);
    addChild(new Line(z, appZ));
  }
}
