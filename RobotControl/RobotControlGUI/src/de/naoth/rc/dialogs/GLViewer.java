/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.dialogs;

import com.jogamp.opengl.GL3;
import com.jogamp.opengl.GLAutoDrawable;
import com.jogamp.opengl.GLCapabilities;
import com.jogamp.opengl.GLEventListener;
import com.jogamp.opengl.GLProfile;
import com.jogamp.opengl.awt.GLCanvas;
import com.jogamp.opengl.util.FPSAnimator;
import de.naoth.rc.opengl.Camera;
import de.naoth.rc.opengl.file.GLScene;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.representations.Point3f;
import de.naoth.rc.opengl.representations.Vector3f;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.manager.GLViewerSceneManager;
import de.naoth.rc.manager.ImageManagerBottom;
import de.naoth.rc.manager.ImageManagerTop;
import de.naoth.rc.opengl.drawings.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.LinkedList;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

//@PluginImplementation
public class GLViewer extends AbstractDialog
    implements ObjectListener<GLDrawable[]> {

    @PluginImplementation
    public static class Plugin extends DialogPlugin<GLViewer> {

        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static GLViewerSceneManager glViewerSceneManager;
        @InjectPlugin
        public static ImageManagerBottom imageManager;
        @InjectPlugin
        public static ImageManagerTop imageTopManager;
    }//end Plugin

    private final GLCanvas canvas;
    
    private GLEventListenerImpl glImpl;

    /**
     * Creates new form ThreeDimensionViewer
     */
    public GLViewer() {
        super();
        initComponents();

        GLProfile glp = GLProfile.getGL2GL3();
        GLCapabilities caps = new GLCapabilities(glp);

        this.canvas = new GLCanvas(caps);
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jToolBar = new javax.swing.JToolBar();
        jToggleButtonUpdate = new javax.swing.JToggleButton();
        jCheckBoxField = new javax.swing.JCheckBox();
        jCheckBoxImage = new javax.swing.JCheckBox();
        cbUseFieldViewer = new javax.swing.JCheckBox();
        jPanelCanvas = new com.jogamp.opengl.awt.GLJPanel();

        jToolBar.setFloatable(false);
        jToolBar.setRollover(true);

        jToggleButtonUpdate.setText("Update");
        jToggleButtonUpdate.setToolTipText("Update scene from robot.");
        jToggleButtonUpdate.setFocusable(false);
        jToggleButtonUpdate.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        jToggleButtonUpdate.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToggleButtonUpdate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jToggleButtonUpdateActionPerformed(evt);
            }
        });
        jToolBar.add(jToggleButtonUpdate);

        jCheckBoxField.setSelected(true);
        jCheckBoxField.setText("Field");
        jCheckBoxField.setToolTipText("Show the soccer field.");
        jCheckBoxField.setFocusable(false);
        jCheckBoxField.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        jCheckBoxField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jCheckBoxFieldActionPerformed(evt);
            }
        });
        jToolBar.add(jCheckBoxField);

        jCheckBoxImage.setText("Image");
        jCheckBoxImage.setToolTipText("Receive image from camera of robot.");
        jCheckBoxImage.setFocusable(false);
        jCheckBoxImage.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        jCheckBoxImage.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jCheckBoxImage.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jCheckBoxImageActionPerformed(evt);
            }
        });
        jToolBar.add(jCheckBoxImage);

        cbUseFieldViewer.setText("Use FieldViewer");
        cbUseFieldViewer.setFocusable(false);
        cbUseFieldViewer.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        cbUseFieldViewer.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToolBar.add(cbUseFieldViewer);

        jPanelCanvas.addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentResized(java.awt.event.ComponentEvent evt) {
                jPanelCanvasComponentResized(evt);
            }
        });

        javax.swing.GroupLayout jPanelCanvasLayout = new javax.swing.GroupLayout(jPanelCanvas);
        jPanelCanvas.setLayout(jPanelCanvasLayout);
        jPanelCanvasLayout.setHorizontalGroup(
            jPanelCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );
        jPanelCanvasLayout.setVerticalGroup(
            jPanelCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 269, Short.MAX_VALUE)
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar, javax.swing.GroupLayout.DEFAULT_SIZE, 400, Short.MAX_VALUE)
            .addComponent(jPanelCanvas, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanelCanvas, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

  private void jCheckBoxFieldActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jCheckBoxFieldActionPerformed
  {//GEN-HEADEREND:event_jCheckBoxFieldActionPerformed
      //TODO do something
  }//GEN-LAST:event_jCheckBoxFieldActionPerformed

  private void jToggleButtonUpdateActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jToggleButtonUpdateActionPerformed
  {//GEN-HEADEREND:event_jToggleButtonUpdateActionPerformed
      //TODO do something
  }//GEN-LAST:event_jToggleButtonUpdateActionPerformed

  private void jCheckBoxImageActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jCheckBoxImageActionPerformed
  {//GEN-HEADEREND:event_jCheckBoxImageActionPerformed
      //TODO do something
  }//GEN-LAST:event_jCheckBoxImageActionPerformed

    private void jPanelCanvasComponentResized(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_jPanelCanvasComponentResized
        canvas.setSize(jPanelCanvas.getSize());
    }//GEN-LAST:event_jPanelCanvasComponentResized

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JCheckBox cbUseFieldViewer;
    private javax.swing.JCheckBox jCheckBoxField;
    private javax.swing.JCheckBox jCheckBoxImage;
    private com.jogamp.opengl.awt.GLJPanel jPanelCanvas;
    private javax.swing.JToggleButton jToggleButtonUpdate;
    private javax.swing.JToolBar jToolBar;
    // End of variables declaration//GEN-END:variables

    @Override
    public void init() {
        Input inputListener = new Input();
        this.canvas.addKeyListener((KeyListener) inputListener);
        this.canvas.addMouseListener((MouseListener) inputListener);

        glImpl = new GLEventListenerImpl(inputListener, canvas);

        this.canvas.addGLEventListener(glImpl);

        jPanelCanvas.add(this.canvas);
    }

    @Override
    public void dispose() {
        System.out.println("Dispose is not implemented for: " + this.getClass().getName());
    }//end dispose

    @Override
    public void newObjectReceived(GLDrawable[] object) {
        glImpl.scene.add(object);
    }

    @Override
    public void errorOccured(String cause) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

}

final class GLEventListenerImpl implements GLEventListener {

    private FPSAnimator animator;

    private final Input inputListener;

    private GL3 gl;

    private Camera camera;

    private final GLCanvas canvas;
    
    public GLScene scene;

    private final Point3f camPos = new Point3f(20, 30, 40);

    public GLEventListenerImpl(Input inputListener, GLCanvas canvas) {
        this.canvas = canvas;

        this.inputListener = inputListener;
    }

    @Override
    public void init(GLAutoDrawable drawable) {
        gl = drawable.getGL().getGL3();

        String scenePath = System.getProperty("user.dir").replaceAll("\\\\", "/") + "/src/de/naoth/rc/opengl/res/";

        scene = new GLScene(gl);

        //scene.add(scenePath + "scene.simgl");
        //scene.add(new ExampleGLDrawable());
        scene.add(new FieldDrawingSPL2013());
        scene.add(new Head());
        /*
        scene.add(new Torso());
        scene.add(new LThigh());
        scene.add(new RThigh());
        scene.add(new LFoot());
        scene.add(new RFoot());
        scene.add(new LShinebone());
        scene.add(new RShinebone());
         */

        gl.glEnable(GL3.GL_DEPTH_TEST);

        animator = new FPSAnimator(drawable, 60);
        animator.start();

        camera = new Camera(Camera.FOCUS_MODE, camPos, new Point3f(0, 0, 0), new Point3f(0, 1, 0), canvas.getWidth(), canvas.getHeight());
    }

    @Override
    public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height) {
        camera.reshape(width, height);
    }

    @Override
    public void dispose(GLAutoDrawable drawable) {
        animator.stop();

        for (GLObject each : scene.getRunQueue()) {
            each.dispose();
        }
    }

    @Override
    public void display(GLAutoDrawable drawable) {
        gl.glEnable(GL3.GL_BLEND);
        gl.glBlendFunc(GL3.GL_SRC_ALPHA, GL3.GL_ONE_MINUS_SRC_ALPHA);

        gl.glClearColor(0.839f, 0.851f, 0.875f, 1.0f);
        //gl.glClearColor((float)Math.random(), (float)Math.random(), (float)Math.random(), 1.0f);
        gl.glClear(GL3.GL_COLOR_BUFFER_BIT | GL3.GL_DEPTH_BUFFER_BIT);

        if (inputListener.a) {
            camera.rotateRight(-0.02f);
        }
        if (inputListener.d) {
            camera.rotateRight(0.02f);
        }
        if (inputListener.w) {
            camera.rotateUp(-0.02f);
        }
        if (inputListener.s) {
            camera.rotateUp(0.02f);
        }
        if (inputListener.f) {
            camera.setMode(Camera.FLY_MODE);
        }
        if (inputListener.r) {
            camera.setFocus(new Vector3f(0, 2, 0));
        }
        if (inputListener.up) {
            camera.moveForward(1);
        }
        if (inputListener.down) {
            camera.moveForward(-1);
        }
        if (inputListener.escape) {
            camera = new Camera(Camera.FOCUS_MODE, camPos, new Point3f(0, 0, 0), new Point3f(0, 1, 0), canvas.getWidth(), canvas.getHeight());
        }

        camera.review();

        for (GLObject each : scene.getRunQueue()) {
            each.bindShader();
            each.bind();
            each.display(camera.getCameraMatrix());
            each.unbind();
            each.unbindShader();
        }
    }

}

final class Input implements KeyListener, MouseListener {

    public boolean w, a, s, d, r, f,
        up, down, right, left,
        escape;

    public Input() {
        w = a = s = d = r = f = up = down = right = left = escape = false;
    }

    //KEYLISTENER
    @Override
    public void keyTyped(KeyEvent e) {
    }

    @Override
    public void keyPressed(KeyEvent e) {
        switch (e.getKeyCode()) {
            case KeyEvent.VK_W:
                w = true;
                break;
            case KeyEvent.VK_A:
                a = true;
                break;
            case KeyEvent.VK_S:
                s = true;
                break;
            case KeyEvent.VK_D:
                d = true;
                break;
            case KeyEvent.VK_R:
                r = true;
                break;
            case KeyEvent.VK_F:
                f = true;
                break;
            case KeyEvent.VK_UP:
                up = true;
                break;
            case KeyEvent.VK_DOWN:
                down = true;
                break;
            case KeyEvent.VK_RIGHT:
                right = true;
                break;
            case KeyEvent.VK_LEFT:
                left = true;
                break;
            case KeyEvent.VK_ESCAPE:
                escape = true;
                break;
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        switch (e.getKeyCode()) {
            case KeyEvent.VK_W:
                w = false;
                break;
            case KeyEvent.VK_A:
                a = false;
                break;
            case KeyEvent.VK_S:
                s = false;
                break;
            case KeyEvent.VK_D:
                d = false;
                break;
            case KeyEvent.VK_R:
                r = false;
                break;
            case KeyEvent.VK_F:
                f = false;
                break;
            case KeyEvent.VK_UP:
                up = false;
                break;
            case KeyEvent.VK_DOWN:
                down = false;
                break;
            case KeyEvent.VK_RIGHT:
                right = false;
                break;
            case KeyEvent.VK_LEFT:
                left = false;
                break;
            case KeyEvent.VK_ESCAPE:
                escape = false;
                break;
        }
    }

    //MOUSELISTENER
    @Override
    public void mouseClicked(MouseEvent e) {
        System.out.println(e.getX() + "  " + e.getY());
    }

    @Override
    public void mousePressed(MouseEvent e) {
        // TODO Auto-generated method stub

    }

    @Override
    public void mouseReleased(MouseEvent e) {
        // TODO Auto-generated method stub

    }

    @Override
    public void mouseEntered(MouseEvent e) {
        // TODO Auto-generated method stub

    }

    @Override
    public void mouseExited(MouseEvent e) {
        // TODO Auto-generated method stub

    }
}
