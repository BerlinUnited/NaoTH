/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.hu_berlin.informatik.ki.nao.dialogs.drawings.three_dimension;

import com.interactivemesh.j3d.community.utils.navigation.orbit.OrbitBehaviorInterim;
import java.awt.Component;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import javax.media.j3d.Transform3D;
import javax.media.j3d.TransformGroup;
import javax.media.j3d.View;
import javax.vecmath.Vector3d;
import javax.vecmath.Vector3f;

public class OrbitBehavior extends OrbitBehaviorInterim {
    private static final double ROT_AMT = Math.PI / 180.0 * 3;
    private static final double MOVE_STEP = 0.2;

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

    public OrbitBehavior(Component canvas, TransformGroup viewTG, View view) {
        super(canvas, viewTG, view, OrbitBehaviorInterim.REVERSE_ALL);

        canvas.addKeyListener(new KeyListener() {

            public void keyTyped(KeyEvent e) {
            }

            public void keyPressed(KeyEvent e) {
                int keyCode = e.getKeyCode();
                if (e.isControlDown()){
                    switch (keyCode) {
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
                else{
                    switch (keyCode) {
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

            public void keyReleased(KeyEvent e) {
            }
        });


        rotX(Math.PI*0.5);
    }

    private void move(Vector3d theMove) {
        targetTG.getTransform(t3d);
        toMove.setTranslation(theMove);
        t3d.mul(toMove);
        targetTG.setTransform(t3d);
    }

    // rotate about y-axis by radians
    private void rotY(double radians) {
        targetTG.getTransform(t3d);   // targetTG is the ViewPlatform's tranform
        toRot.rotY(radians);
        t3d.mul(toRot);
        targetTG.setTransform(t3d);
    }

    private void rotX(double radians){
        targetTG.getTransform(t3d);   // targetTG is the ViewPlatform's tranform
        toRot.rotX(radians);
        t3d.mul(toRot);
        targetTG.setTransform(t3d);
    }

    public void setHomePosition(Vector3f p){
        Transform3D t = new Transform3D();
        t.rotX(Math.PI*0.5);
        t.setTranslation(new Vector3f(p));
        setHomeTransform(t);
    }
}
