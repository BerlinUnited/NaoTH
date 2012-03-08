/* @file ChessFloor.java
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 * @breief define the key and mouse behavior
 */

package de.naoth.me.emulation;

import com.interactivemesh.j3d.community.utils.navigation.orbit.OrbitBehaviorInterim;
import javax.media.j3d.Canvas3D;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import javax.media.j3d.Transform3D;
import javax.media.j3d.TransformGroup;
import javax.media.j3d.View;
import javax.vecmath.Vector3d;
import javax.vecmath.Vector3f;

public class OrbitBehavior extends OrbitBehaviorInterim
{
    private static final double ROT_AMT = Math.PI / 180.0 * 4;
    private static final double MOVE_STEP = 0.02;

    private boolean ignoreInput = false;

    // hardwired movement vectors
    private static final Vector3d FWD = new Vector3d(0, 0, -MOVE_STEP);
    private static final Vector3d BACK = new Vector3d(0, 0, MOVE_STEP);
    private static final Vector3d LEFT = new Vector3d(-MOVE_STEP, 0, 0);
    private static final Vector3d RIGHT = new Vector3d(MOVE_STEP, 0, 0);
    private static final Vector3d UP = new Vector3d(0, MOVE_STEP, 0);
    private static final Vector3d DOWN = new Vector3d(0, -MOVE_STEP, 0);

    private Transform3D t3d = new Transform3D();
    private Transform3D toMove = new Transform3D();
    private Transform3D toRot = new Transform3D();
    private Transform3D homePosition = new Transform3D();

    private int mouseClickCount = 0;

    private boolean[] buttonPressed = {false, false, false};
    private KneadEmulator parent = null;


    OrbitBehavior(Canvas3D canvas, TransformGroup viewTG, View view, KneadEmulator parent)
    {
        super(canvas, viewTG, view, OrbitBehaviorInterim.REVERSE_ALL);
        this.parent = parent;

        MouseMotionListener[] allMMLs = canvas.getMouseMotionListeners();
        for(int i = 0; i < allMMLs.length; i++)
        {
          canvas.removeMouseMotionListener(allMMLs[i]);
        }

        canvas.addMouseMotionListener
        (
          new MouseMotionListener()
          {
            int lastX = 0;
            int lastY = 0;

            public void mouseDragged(MouseEvent e)
            {
//              throw new UnsupportedOperationException("Not supported yet.");
              int dX = lastX - e.getX();
              int dY = lastY - e.getY();

              if(dX > 0)
              {
                rotZ(ROT_AMT);
              }
              else if(dX < 0)
              {
                rotZ(-ROT_AMT);
              }
              if(dY > 0)
              {
                if(buttonPressed[0])
                {
                  rotY(ROT_AMT);
                }
                if(buttonPressed[2])
                {
                  rotX(ROT_AMT);
                }
              }
              else if(dY < 0)
              {
                if(buttonPressed[0])
                {
                  rotY(-ROT_AMT);
                }
                if(buttonPressed[2])
                {
                  rotX(-ROT_AMT);
                }
              }
              lastX = e.getX();
              lastY = e.getY();
              
            }

            public void mouseMoved(MouseEvent e)
            {
              mouseClickCount = 0;
            }
          }
        );

        MouseWheelListener[] allMWLs = canvas.getMouseWheelListeners();
        for(int i = 0; i < allMWLs.length; i++)
        {
          canvas.removeMouseWheelListener(allMWLs[i]);
        }

        canvas.addMouseWheelListener
        (
          new MouseWheelListener()
          {
            public void mouseWheelMoved(MouseWheelEvent e)
            {
              int dR = e.getWheelRotation();
              if(dR > 0)
              {
                move(BACK);
              }
              else if(dR < 0)
              {
                move(FWD);
              }
            }
          }
        );

        MouseListener[] allMLs = canvas.getMouseListeners();
        for(int i = 0; i < allMLs.length; i++)
        {
          canvas.removeMouseListener(allMLs[i]);
        }

        canvas.addMouseListener
        (
          new MouseListener()
          {
            public void mouseClicked(MouseEvent e)
            {

              mouseClickCount++;
              if(mouseClickCount >= 2)
              {
                mouseClickCount = 0;
//                getHomeTransform(homePosition);
//                targetTG.setTransform(homePosition);
////                rotX(Math.PI*0.5);
////                 setHomePosition(new Vector3f(2.0f, 0.0f, 0.0f));
                System.out.println("homePosition");
              }
            }

            public void mousePressed(MouseEvent e)
            {
               int pressedButton = e.getButton();
               if(pressedButton == MouseEvent.BUTTON1)
               {
                 if(!buttonPressed[2])
                 {
                  buttonPressed[0] = true;
                 }
                 else
                 {
                   buttonPressed[2] = false;
                 }
               }
               if(pressedButton == MouseEvent.BUTTON3)
               {
                 if(!buttonPressed[0])
                 {
                   buttonPressed[2] = true;
                 }
                 else
                 {
                   buttonPressed[0] = false;
                 }
               }
            }

            public void mouseReleased(MouseEvent e)
            {
              int releasedButton = e.getButton();
              if(releasedButton == MouseEvent.BUTTON1)
              {
                buttonPressed[0] = false;
              }
              if(releasedButton == MouseEvent.BUTTON3)
              {
                buttonPressed[2] = false;
              }
            }

            public void mouseEntered(MouseEvent e)
            {
            }

            public void mouseExited(MouseEvent e)
            {
            }
          }
        );

        canvas.addKeyListener
        (
          new KeyListener()
          {

              public void keyTyped(KeyEvent e)
              {}

              public void keyPressed(KeyEvent e)
              {
                  int keyCode = e.getKeyCode();
                  if (e.isControlDown())
                  {
                      switch (keyCode)
                      {
                          case KeyEvent.VK_UP:
                          case KeyEvent.VK_W:
                              move(UP);
                              break;

                          case KeyEvent.VK_DOWN:
                          case KeyEvent.VK_S:
                              move(DOWN);
                              break;

                          case KeyEvent.VK_LEFT:
                          case KeyEvent.VK_A:
                              move(LEFT);
                              break;

                          case KeyEvent.VK_RIGHT:
                          case KeyEvent.VK_D:
                              move(RIGHT);
                              break;
                      }
                  }
                  else
                  {
                      switch (keyCode)
                      {
                          case KeyEvent.VK_UP:
                          case KeyEvent.VK_W:
                              move(FWD);
                              break;

                          case KeyEvent.VK_DOWN:
                          case KeyEvent.VK_S:
                              move(BACK);
                              break;

                          case KeyEvent.VK_LEFT:
                          case KeyEvent.VK_A:
                              rotY(ROT_AMT);
                              break;

                          case KeyEvent.VK_RIGHT:
                          case KeyEvent.VK_D:
                              rotY(-ROT_AMT);
                              break;

                          case KeyEvent.VK_PAGE_UP:
                          case KeyEvent.VK_Q:
                              move(UP);
                              break;

                          case KeyEvent.VK_PAGE_DOWN:
                          case KeyEvent.VK_E:
                              move(DOWN);
                              break;
                      }
                  }
              }

              public void keyReleased(KeyEvent e)
              {}
          }
        );
        rotX(Math.PI * 0.5);

//        targetTG.getTransform(homePosition);
        this.getHomeTransform(homePosition);
        //this.setHomeTransform(homePosition);

    }

    private void move(Vector3d theMove)
    {
      if(!this.ignoreInput)
      {
        targetTG.getTransform(t3d);
        toMove.setTranslation(theMove);
        t3d.mul(toMove);
        targetTG.setTransform(t3d);
        parent.textMove(theMove);
      }
    }

    // rotate about y-axis by radians
    private void rotY(double radians)
    {
      if(!this.ignoreInput)
      {
        targetTG.getTransform(t3d);   // targetTG is the ViewPlatform's tranform
        toRot.rotY(radians);
        toRot.mul(t3d);
        targetTG.setTransform(toRot);
        parent.textRotY(radians);
      }
    }

    private void rotX(double radians)
    {
      if(!this.ignoreInput)
      {
        targetTG.getTransform(t3d);   // targetTG is the ViewPlatform's tranform
        toRot.rotX(radians);
        toRot.mul(t3d);
        targetTG.setTransform(toRot);
        parent.textRotX(radians);
      }
    }

    private void rotZ(double radians)
    {
      if(!this.ignoreInput)
      {
        targetTG.getTransform(t3d);   // targetTG is the ViewPlatform's tranform
        toRot.rotZ(radians);
        toRot.mul(t3d);
        targetTG.setTransform(toRot);
        parent.textRotZ(radians);
      }
    }

    public void setHomePosition(Vector3f p)
    {
//      if(!this.ignoreInput)
      {
        Transform3D t = new Transform3D();
        t.rotX(Math.PI*0.5);
        t.setTranslation(new Vector3f(p));
        setHomeTransform(t);
      }
    }

    public void setIgnoreInput(boolean ignore)
    {
      this.ignoreInput = ignore;
    }
}
