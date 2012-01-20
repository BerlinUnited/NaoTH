/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.dialogs.drawings.three_dimension;

import de.naoth.rc.dataformats.JanusImage;
import java.util.zip.DataFormatException;
import javax.media.j3d.Appearance;
import javax.media.j3d.ImageComponent2D;
import javax.media.j3d.QuadArray;
import javax.media.j3d.Shape3D;
import javax.media.j3d.Texture2D;
import javax.media.j3d.Transform3D;
import javax.vecmath.Point3f;
import javax.vecmath.Matrix3f;
import javax.vecmath.Vector3f;
import javax.vecmath.TexCoord2f;

public class Camera extends Drawable
{

  private Point3f[] vertex = new Point3f[4];

  public Camera(String[] tokens) throws DataFormatException
  {
    float scale = 0.0012f;
    if(tokens.length > 4 && tokens[0].equals("Camera"))
    {
      float focusLength = Float.parseFloat(tokens[1]) * scale;
      float width = Float.parseFloat(tokens[2]) * scale;
      float height = Float.parseFloat(tokens[3]) * scale;

      Transform3D cameraTransform = parseTransform3D(tokens, 4);

      if(tokens.length > 13 && tokens[13].equals("Robot"))
      {
        Transform3D robotTransform;
        robotTransform = parseTransform3D(tokens, 14);

        Matrix3f rotRobot = new Matrix3f();
        Vector3f transRobot = new Vector3f();
        robotTransform.get(rotRobot, transRobot);

        Matrix3f rotCamera = new Matrix3f();
        Vector3f transCamera = new Vector3f();
        robotTransform.get(rotCamera, transCamera);

        Transform3D toRot = new Transform3D();
        toRot.setRotation(rotCamera);

        Transform3D toMove = new Transform3D();
        toMove.setTranslation(transRobot);
        robotTransform.mul(cameraTransform);
        setTransform(robotTransform);
      }
      else
      {
        setTransform(cameraTransform);
      }

      float w_2 = width / 2;
      float h_2 = height / 2;
      vertex[0] = new Point3f(focusLength, -w_2,  h_2);
      vertex[1] = new Point3f(focusLength,  w_2,  h_2);
      vertex[2] = new Point3f(focusLength,  w_2, -h_2);
      vertex[3] = new Point3f(focusLength, -w_2, -h_2);

      Appearance app = parseAppearance("FFFFFF");
      Point3f[] ps = new Point3f[16];
      ps[0] = new Point3f(0.0f, 0.0f, 0.0f);
      ps[1] = vertex[0];
      ps[2] = ps[0];
      ps[3] = vertex[1];
      ps[4] = ps[0];
      ps[5] = vertex[2];
      ps[6] = ps[0];
      ps[7] = vertex[3];
      ps[8] = vertex[0];
      ps[9] = vertex[1];
      ps[10] = vertex[1];
      ps[11] = vertex[2];
      ps[12] = vertex[2];
      ps[13] = vertex[3];
      ps[14] = vertex[3];
      ps[15] = vertex[0];
      addChild(new Line(ps, app));
    }
  }

  public void addImage(JanusImage image)
  {
    if (null != image && vertex.length == 4)
    {
      ImageComponent2D imageComponent = new ImageComponent2D(ImageComponent2D.FORMAT_RGB, image.getRgb());
      QuadArray rect = new QuadArray(4, QuadArray.COORDINATES | QuadArray.TEXTURE_COORDINATE_2);
      rect.setCoordinate(0, vertex[0]);
      rect.setCoordinate(1, vertex[1]);
      rect.setCoordinate(2, vertex[2]);
      rect.setCoordinate(3, vertex[3]);

      rect.setTextureCoordinate(0, 0, new TexCoord2f(1.0f, 1.0f));
      rect.setTextureCoordinate(0, 1, new TexCoord2f(0.0f, 1.0f));
      rect.setTextureCoordinate(0, 2, new TexCoord2f(0.0f, 0.0f));
      rect.setTextureCoordinate(0, 3, new TexCoord2f(1.0f, 0.0f));

      Texture2D tex = new Texture2D(Texture2D.INTENSITY, Texture2D.RGB, imageComponent.getWidth(), imageComponent.getHeight());
      tex.setImage(0, imageComponent);
      Appearance app = new Appearance();
      app.setTexture(tex);
      Shape3D s = new Shape3D(rect, app);
      this.addChild(s);
    }
  }
}
