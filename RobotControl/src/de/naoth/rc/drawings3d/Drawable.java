/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

package de.naoth.rc.drawings3d;

import de.naoth.rc.drawings.Colors;
import java.awt.Color;
import java.util.zip.DataFormatException;

import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.ColoringAttributes;
import org.jogamp.java3d.Material;
import org.jogamp.java3d.Transform3D;
import org.jogamp.java3d.TransformGroup;

import org.jogamp.vecmath.Color3f;

public class Drawable extends TransformGroup
{

  protected float[] parseFloatArray(String[] tokens, int beg, int end)
  {
    float[] dim = new float[end-beg];
    for(int i = 0; i < dim.length; i++) {
      dim[i] = Float.parseFloat(tokens[beg+i]) * 0.001f;
    }
    return dim;
  }
  
  protected Color3f parseColor(String color) {
      Color javaColor = Colors.parseColor(color);
      return new Color3f(javaColor.getRGBColorComponents(null));
  }
  
  protected Appearance parseAppearance(String color)
  {
    Color3f c = parseColor(color);
    
    Appearance app = new Appearance();
    app.setColoringAttributes(new ColoringAttributes(c, ColoringAttributes.SHADE_FLAT));

    Material mat = new Material();
    mat.setAmbientColor(c);
    mat.setDiffuseColor(c);
    mat.setSpecularColor(c);
    mat.setColorTarget(Material.AMBIENT_AND_DIFFUSE);
    app.setMaterial(mat);
    
    /*
    app.setPolygonAttributes(new PolygonAttributes(
          PolygonAttributes.POLYGON_LINE,
          PolygonAttributes.CULL_NONE, 0));
    */
    return app;
  }

  protected Transform3D parseTransform3D(String[] ds, int offset) throws DataFormatException
  {
    if (ds.length < 9 + offset)
    {
      throw new DataFormatException("Transform3D needs 9 elements!");
    }

    double[] m = new double[16];
    int k = offset;
    for (int j = 0; j < 12; j += 4)
    {
      m[j] = Double.parseDouble(ds[k++]);
      m[j + 1] = Double.parseDouble(ds[k++]);
      m[j + 3] = 0;
    }

    m[2] = m[4] * m[9] - m[5] * m[8];
    m[6] = m[1] * m[8] - m[0] * m[9];
    m[10] = m[0] * m[5] - m[1] * m[4];
    m[11] = 0;
    m[12] = Double.parseDouble(ds[k++]) * 0.001;
    m[13] = Double.parseDouble(ds[k++]) * 0.001;
    m[14] = Double.parseDouble(ds[k++]) * 0.001;
    m[15] = 1;
    Transform3D t3d = new Transform3D(m);
    t3d.transpose();
    return t3d;
  }
}
