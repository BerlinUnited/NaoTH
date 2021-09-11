/*
 * 
 */

package de.naoth.rc.drawings3d;

import javax.media.j3d.Appearance;
import javax.media.j3d.ColoringAttributes;
import javax.media.j3d.Material;
import javax.media.j3d.Shape3D;
import javax.media.j3d.Transform3D;
import javax.media.j3d.TriangleStripArray;
import javax.vecmath.Color3f;
import javax.vecmath.Point3f;
import javax.vecmath.Vector3f;

/**
 *
 * @author Heinrich Mellmann
 */
public class CircularConstraint3D extends Drawable
{
  double x;
  double y;
  double rotation;

  double distance = 1;
  double deviation = 0.2;

  double angle = Math.PI/2;
  double angleDeviation = Math.PI/8;

  public CircularConstraint3D(double x, double y, double rotation)
  {
    Color3f c = new Color3f(1f,0f,0f);

    Appearance app = new Appearance();
    app.setColoringAttributes(new ColoringAttributes(c, ColoringAttributes.SHADE_FLAT));

    Material mat = new Material();
    mat.setAmbientColor(c);
    mat.setDiffuseColor(c);
    mat.setSpecularColor(new Color3f(1.0f, 1.0f,1.0f));
    mat.setColorTarget(Material.AMBIENT_AND_DIFFUSE);
    app.setMaterial(mat);
    
    Transform3D t = new Transform3D();
    t.setTranslation(new Vector3f((float)x,(float)y,0f));

    setTransform(t);

    
    int number = 20;
    int pointNumber = (number+1)*2;
    int stripzahl[] = {pointNumber};
    Point3f ecke[] = new Point3f[pointNumber];
    Point3f eckeBottom[] = new Point3f[pointNumber];
    Point3f eckeInner[] = new Point3f[pointNumber];
    Point3f eckeOuter[] = new Point3f[pointNumber];
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    
    for(double alpha = -Math.PI; alpha <= Math.PI+0.001; alpha += 2*Math.PI/number)
    {
      double alpha0 = Math.PI + alpha + angle;
              
      double xp0 = Math.cos(alpha0)*(distance-deviation);
      double yp0 = Math.sin(alpha0)*(distance-deviation);
      double xp1 = Math.cos(alpha0)*(distance+deviation);
      double yp1 = Math.sin(alpha0)*(distance+deviation);
      
      double zp0 = (Math.PI + alpha)/(2*Math.PI);
      double zp1 = (Math.PI + alpha+2*angleDeviation)/(2*Math.PI);;


      {
        Point3f inner = new Point3f((float)(xp0),(float)(yp0), (float)(zp0));
        Point3f outer = new Point3f((float)(xp1),(float)(yp1), (float)(zp0));

        ecke[i++] = outer;
        ecke[i++] = inner;

        eckeInner[k++] = inner;
        eckeOuter[l+1] = outer;
      }
      {
        Point3f inner = new Point3f((float)(xp0),(float)(yp0), (float)(zp1));
        Point3f outer = new Point3f((float)(xp1),(float)(yp1), (float)(zp1));
        eckeBottom[j++] = inner;
        eckeBottom[j++] = outer;

        eckeInner[k++] = inner;
        eckeOuter[l++] = outer;
        l++;
      }
      
    }//end for

    
    TriangleStripArray top = new TriangleStripArray(pointNumber*2,
            TriangleStripArray.COORDINATES|TriangleStripArray.NORMALS, stripzahl);
    top.setCoordinates(0, ecke);
    
    TriangleStripArray bottom = new TriangleStripArray(pointNumber*2,
            TriangleStripArray.COORDINATES|TriangleStripArray.NORMALS, stripzahl);
    bottom.setCoordinates(0, eckeBottom);

    TriangleStripArray inner = new TriangleStripArray(pointNumber*2,
            TriangleStripArray.COORDINATES|TriangleStripArray.NORMALS, stripzahl);
    inner.setCoordinates(0, eckeInner);

    TriangleStripArray outer = new TriangleStripArray(pointNumber*2,
            TriangleStripArray.COORDINATES|TriangleStripArray.NORMALS, stripzahl);
    outer.setCoordinates(0, eckeOuter);

    addChild(new Shape3D(bottom, app));
    addChild(new Shape3D(top, app));
    addChild(new Shape3D(inner, app));
    addChild(new Shape3D(outer, app));
  }
}//end CircularConstraint3D
