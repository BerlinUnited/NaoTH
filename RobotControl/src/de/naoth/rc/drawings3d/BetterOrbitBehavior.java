/*
* BetterOrbitBehavior wrappes OrbitBehavior. This fulflls two functions.
* 
*  1. adapt to JCanvas3D - originally, OrbitBehavior works only with Canvas3D.
*  2. reimplement movement behavior to make it feel more natural.
*/
package de.naoth.rc.drawings3d;

import java.awt.AWTEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import javax.swing.SwingUtilities;

import org.jogamp.java3d.exp.swing.JCanvas3D;
import org.jogamp.java3d.utils.behaviors.vp.OrbitBehavior;


import org.jogamp.java3d.BranchGroup;
import org.jogamp.java3d.TransformGroup;
import org.jogamp.java3d.Transform3D;
import org.jogamp.vecmath.Vector3d;
import org.jogamp.vecmath.Point3d;

import java.awt.event.KeyListener;

/**
 *
 * @author Heinrich
 */
public class BetterOrbitBehavior extends OrbitBehavior
{   
    private final JCanvas3D canvas;

    /**
    * Used to set the function for a mouse button to Zoom
    */
    private static final int ZOOM = 2;
    
    private int wheelZoomFactor = 50;

    private static final double NOMINAL_ZOOM_FACTOR  = 0.01;
    private static final double NOMINAL_ROT_FACTOR   = 0.01;
    private static final double NOMINAL_TRANS_FACTOR = 0.01;
    
    private int mouseX = 0;
    private int mouseY = 0;
    
    private int transationXpx = 0;
    private int transationYpx = 0;
    
    // rotation
    private double mouseRotationX = 0;
    private double mouseRotationY = 0;
    
    // zoom
    private double distanceFromCenter = 10.0;
    
    // translation
    private double transationX = 0;
    private double transationY = 0;
    
    // for debug purposes
    private final TransformGroup cubeLocation = new TransformGroup();
    
    public BetterOrbitBehavior(JCanvas3D canvas) {
        this(canvas, null);
    }
    
    
    public BetterOrbitBehavior(JCanvas3D canvas, BranchGroup root) {
        super();
        
        this.canvas = canvas;
        
        if(root != null) {
            org.jogamp.java3d.utils.geometry.ColorCube cb = new org.jogamp.java3d.utils.geometry.ColorCube(0.1);
            cubeLocation.addChild(cb);
            cubeLocation.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
            root.addChild(cubeLocation);
        }
    }
    
    @Override
    public void setEnable(boolean state) {
        super.setEnable(state);
        
        // manually add listeners to the JCanvas3D because the parent class only works with Canvas3D
        canvas.addMouseListener(this);
        canvas.addMouseMotionListener(this);
        canvas.addMouseWheelListener(this);
        canvas.addKeyListener(this);
        canvas.addKeyListener(this);
    }
    

    @Override
    protected synchronized void processAWTEvents( final AWTEvent[] events ) {
        motion = false;
        for(int i=0; i<events.length; i++) {
            if (events[i] instanceof MouseEvent) {
                processMouseEvent( (MouseEvent)events[i] );
            } else if(events[i] instanceof KeyEvent) {
                processKeyEvent( (KeyEvent)events[i] );
            }
        }
    }
    
    
    protected void processKeyEvent(final KeyEvent e) 
    {
        if(e.getID() != KeyEvent.KEY_PRESSED) {
            return;
        }
        
        int keyCode = e.getKeyCode();
        
        if (e.isControlDown())
        {
            switch (keyCode) 
            {
                case KeyEvent.VK_UP:
                case KeyEvent.VK_W:
                    //move(UP);
                    transationXpx = 0;
                    transationYpx = 20;
                    motion = true;
                    break;
                case KeyEvent.VK_DOWN:
                case KeyEvent.VK_S:
                    //move(DOWN);
                    transationXpx = 0;
                    transationYpx = -20;
                    motion = true;
                    break;
                case KeyEvent.VK_LEFT:
                case KeyEvent.VK_A:
                    //move(LEFT);
                    transationXpx = -20;
                    transationYpx = 0;
                    motion = true;
                    break;
                case KeyEvent.VK_RIGHT:
                case KeyEvent.VK_D:
                    //move(RIGHT);
                    transationXpx = 20;
                    transationYpx = 0;
                    motion = true;
                    break;
            }
        }
        else
        {   
            double xchange = 0;
            double ychange = 0;
            
            switch (keyCode) 
            {
                case KeyEvent.VK_UP:
                case KeyEvent.VK_W:
                    //move(FWD);
                    xchange =   0;
                    ychange =  20;
                    break;
                case KeyEvent.VK_DOWN:
                case KeyEvent.VK_S:
                    //move(BACK);
                    xchange =   0;
                    ychange = -20;
                    break;
                case KeyEvent.VK_LEFT:
                case KeyEvent.VK_A:
                    //rotY(ROT_AMT);
                    xchange = -20;
                    ychange =   0;
                    break;
                case KeyEvent.VK_RIGHT:
                case KeyEvent.VK_D:
                    //rotY(-ROT_AMT);
                    xchange = 20;
                    ychange =  0;
                    break;
                case KeyEvent.VK_PAGE_UP:
                case KeyEvent.VK_Q: {
                    //move(UP);
                    int wheelRotation = +1;
                    int zoom = wheelZoomFactor * wheelRotation;

                    double zoomMul = NOMINAL_ZOOM_FACTOR * getZoomFactor();
                    distanceFromCenter += zoom*zoomMul;
                } break;
                case KeyEvent.VK_PAGE_DOWN:
                case KeyEvent.VK_E: {
                    //move(DOWN);
                    int wheelRotation = -1;
                    int zoom = wheelZoomFactor * wheelRotation;

                    double zoomMul = NOMINAL_ZOOM_FACTOR * getZoomFactor();
                    distanceFromCenter += zoom*zoomMul;
                } break;
            }
            
            double rotXMul = NOMINAL_ROT_FACTOR*getRotXFactor();
            double rotYMul = NOMINAL_ROT_FACTOR*getRotYFactor();

            mouseRotationX -= xchange * rotXMul;
            mouseRotationY -= ychange * rotYMul;

            // constrain
            mouseRotationY = Math.max(mouseRotationY, 0);
            mouseRotationY = Math.min(mouseRotationY, Math.PI);
        }

        //integrateTransforms();
        motion = true;
    }
    
    @Override
    protected void processMouseEvent( final MouseEvent evt ) 
    {
        switch (evt.getID()) 
        {
            case MouseEvent.MOUSE_PRESSED:
                mouseX = evt.getX();
                mouseY = evt.getY();
                motion = true;
                break;
                
            case MouseEvent.MOUSE_DRAGGED:
                int xchange = evt.getX() - mouseX;
                int ychange = evt.getY() - mouseY;
                
                // handle translation
                if (evt.isControlDown() || SwingUtilities.isRightMouseButton(evt)) 
                {
                    double transXMul = NOMINAL_TRANS_FACTOR*getTransXFactor();
                    double transYMul = NOMINAL_TRANS_FACTOR*getTransYFactor();
                    
                    transationXpx += xchange;
                    transationYpx += ychange;
                    
                    transationX -= xchange * transXMul;
                    transationY += ychange * transYMul;
                }
                else if (evt.isAltDown() || SwingUtilities.isMiddleMouseButton(evt)) 
                {
                    double zoomMul = NOMINAL_ZOOM_FACTOR * getZoomFactor();
                    distanceFromCenter += ychange*zoomMul;
                }
                // handle rotation
                else 
                {
                    double rotXMul = NOMINAL_ROT_FACTOR*getRotXFactor();
                    double rotYMul = NOMINAL_ROT_FACTOR*getRotYFactor();
                    
                    mouseRotationX -= xchange * rotXMul;
                    mouseRotationY -= ychange * rotYMul;
                    
                    // constrain
                    mouseRotationY = Math.max(mouseRotationY, 0);
                    mouseRotationY = Math.min(mouseRotationY, Math.PI);
                }
                
                mouseX = evt.getX();
                mouseY = evt.getY();
                motion = true;
                break;
            case MouseEvent.MOUSE_RELEASED:
                break;
            case MouseEvent.MOUSE_WHEEL:
                if ( evt instanceof MouseWheelEvent) {
                    int wheelRotation = ((MouseWheelEvent)evt).getWheelRotation();
                    int zoom = wheelZoomFactor * wheelRotation;
                    
                    double zoomMul = NOMINAL_ZOOM_FACTOR * getZoomFactor();
                    distanceFromCenter += zoom*zoomMul;
                    
                    motion = true;
                }
                break;
            default:
                break;
        }
   }
    
    
    class GroundProjector 
    {
        private final Transform3D cameraMatrix  = new Transform3D();
        
        // temporary variables for faster calculations
        private final Point3d pointInImage  = new Point3d();
        private final Point3d pointInWorld  = new Point3d();

        Point3d ccp = new Point3d();
        Vector3d cc = new Vector3d();
        
        // update precalculated values for this frame
        void update() {
            // get current camera matrix
            canvas.getOffscreenCanvas3D().getImagePlateToVworld(cameraMatrix);
            
            Point3d ccpImage = new Point3d();
            canvas.getOffscreenCanvas3D().getCenterEyeInImagePlate(ccpImage);
            cameraMatrix.transform(ccpImage, ccp);
            cc = new Vector3d(ccp);
        }
        
        void project(int x, int y, Vector3d result) {
            canvas.getOffscreenCanvas3D().getPixelLocationInImagePlate(x, y, pointInImage);
            cameraMatrix.transform(pointInImage, pointInWorld);

            // vector from the center towards the ground in world coordinates
            pointInWorld.sub(cc);
            
            if (Math.abs(pointInWorld.z) > 1.e-3) {
                double t = -cc.z / pointInWorld.z;
                pointInWorld.scale(t);
                result.add(cc, pointInWorld);
            }
        }
    }

    private final GroundProjector projector = new GroundProjector();
    
    private final Transform3D rotateTransformX = new Transform3D();
    private final Transform3D rotateTransformZ = new Transform3D();
    private final Transform3D rotateTransform  = new Transform3D();
    
    private final Transform3D zoomTransform  = new Transform3D();
    
    
    // temporary variables
    private final Transform3D translateTransform  = new Transform3D();
    
    private final Vector3d translationStart  = new Vector3d();
    private final Vector3d translationEnd  = new Vector3d();
    private final Vector3d translationDelta = new Vector3d();
    
    // hold acumulate tranlation (also rotation center)
    private final Vector3d center = new Vector3d();
    
    
    @Override
    protected void integrateTransforms() 
    {
        if(targetTG == null) {
            return;
        }
        
        projector.update();
        
        // 1. reset
        targetTransform.setIdentity();
        
        // 2. zoom
        zoomTransform.setTranslation(new Vector3d(0,0,distanceFromCenter));
        targetTransform.mul(targetTransform, zoomTransform);
        
        // 3. rotation
        // compute rotation matrices
        rotateTransformX.rotX(mouseRotationY);
        rotateTransformZ.rotZ(mouseRotationX);
        rotateTransform.mul(rotateTransformZ, rotateTransformX);
        
        // apply rotation
        targetTransform.mul(rotateTransform, targetTransform);
        
        // 4. translation
        if(Math.abs(mouseRotationY*2.0 - Math.PI) > 0.1) {
            projector.project(mouseX - transationXpx, mouseY - transationYpx, translationStart);
            projector.project(mouseX                , mouseY                , translationEnd);
            translationDelta.sub(translationStart, translationEnd);

            center.add(translationDelta);
        }
        
        translateTransform.set(center);
        targetTransform.mul(translateTransform, targetTransform);
        
        // apply the final target tranform
        targetTG.setTransform(targetTransform);
        
        transationX = 0;
        transationY = 0;
        
        transationXpx = 0;
        transationYpx = 0;
        
        /*
        { // debug
            canvas.getOffscreenCanvas3D().getImagePlateToVworld(cameraMatrix);
            
            canvas.getOffscreenCanvas3D().getCenterEyeInImagePlate(ccpImage);
            cameraMatrix.transform(ccpImage, ccp);
            cc = new Vector3d(ccp);
            
            Vector3d centerVector = new Vector3d(0,0,1);
            Vector3d v3 = new Vector3d();
            cameraMatrix.transform(centerVector, v3);
            
            double t3 = -cc.z / v3.z;
            
            cubeLocationTransform.setTranslation(new Vector3d(cc.x + t3*v3.x, cc.y + t3*v3.y, cc.z + t3*v3.z));
            cubeLocation.setTransform(cubeLocationTransform);
        }*/
    }
    
}
