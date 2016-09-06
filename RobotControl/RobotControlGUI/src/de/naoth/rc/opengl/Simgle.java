package de.naoth.rc.opengl;

import com.jogamp.opengl.GL3;
import com.jogamp.opengl.GLAutoDrawable;
import com.jogamp.opengl.GLCapabilities;
import com.jogamp.opengl.GLEventListener;
import com.jogamp.opengl.GLProfile;
import com.jogamp.opengl.awt.GLCanvas;
import com.jogamp.opengl.util.FPSAnimator;
import de.naoth.rc.opengl.file.GLScene;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.representations.Point3f;
import de.naoth.rc.opengl.representations.Vector3f;

import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.LinkedList;

import javax.swing.JFrame;

/**
 * SIMple openGL Example and test program
 */
final class Simgle {

    static GraphicsDevice device = GraphicsEnvironment
            .getLocalGraphicsEnvironment().getScreenDevices()[0];

    public static void main(String[] args) {
        int width = 1920;
        int height = 1080;

        // New FullScreenWindow
        final JFrame frame = new JFrame("simgle");

        GLProfile glp = GLProfile.getGL2GL3();
        GLCapabilities caps = new GLCapabilities(glp);
       
        final GLCanvas canvas = new GLCanvas(caps);
        
        Input inputListener = new Input();
        canvas.addKeyListener((KeyListener)inputListener);
        canvas.addMouseListener((MouseListener)inputListener);
        
        GLDraw gLEventListener = new GLDraw(canvas, inputListener);
        canvas.setSize(width, height);
        canvas.addGLEventListener(gLEventListener);
        
        frame.getContentPane().add(canvas);
        
        frame.addWindowListener(new WindowAdapter() {
            public void windowClosing( WindowEvent windowevent ) {
                frame.remove(canvas);
                frame.dispose();
                System.exit( 0 );
            }
        });
        // TODO fix fullscreen
        // device.setFullScreenWindow(frame);
        frame.setSize(width, height);
        frame.setUndecorated(true);
        frame.setVisible(true);
    }
}


final class GLDraw implements GLEventListener, Computable{
    private FPSAnimator animator;

    private GLCanvas canvas;
    private Input inputListener;
    
    private GL3 gl;
    
    private LinkedList<GLObject> runQueue;
    private Camera camera;
    private Point3f camPos = new Point3f(10,5,40);

    public GLDraw(GLCanvas canvas, Input inputListener) {
        this.canvas = canvas;
        this.inputListener = inputListener;
    }

    @Override
    public void init(GLAutoDrawable drawable) {    	
        gl = drawable.getGL().getGL3();
        
    	String scenePath = System.getProperty("user.dir").replaceAll("\\\\", "/") + "/src/de/naoth/rc/opengl/res/";
    	GLScene scene = new GLScene(gl, scenePath + "scene.simgl");
    	runQueue = scene.getModelList();      

        gl.glEnable(GL3.GL_DEPTH_TEST);

        animator = new FPSAnimator(drawable, 60);
        animator.start();

        camera = new Camera(Camera.FOCUS_MODE, camPos, new Point3f(0, 2, 0), new Point3f(0,1,0), canvas);
    }

    @Override
    public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height) {
        camera.reshape();
    }

    @Override
    public void dispose(GLAutoDrawable drawable) {
        animator.stop();
        
        for (GLObject each: runQueue) {
        	each.dispose();
        }
    }

    @Override
    public void display(GLAutoDrawable drawable) {    	
        gl.glEnable(GL3.GL_BLEND);
        gl.glBlendFunc(GL3.GL_SRC_ALPHA, GL3.GL_ONE_MINUS_SRC_ALPHA);

        gl.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //gl.glClearColor((float)Math.random(), (float)Math.random(), (float)Math.random(), 1.0f);
        gl.glClear(GL3.GL_COLOR_BUFFER_BIT | GL3.GL_DEPTH_BUFFER_BIT);
        
        
        
        
    	if(inputListener.a) {
    		camera.rotateRight(-0.02f);
    	}
    	if(inputListener.d) {
    		camera.rotateRight(0.02f);
    	}
    	if(inputListener.w) {  		
    		camera.rotateUp(-0.02f);
    	}
    	if(inputListener.s) {
    		camera.rotateUp(0.02f);
    	}
    	if(inputListener.f) {
    		camera.setMode(Camera.FLY_MODE);
    	}
    	if(inputListener.r) {
    		camera.setFocus(new Vector3f(0,2,0));
    	}
    	if(inputListener.up) {
    		camera.moveForward(1);
    	}
    	if(inputListener.down) {
    		camera.moveForward(-1);
    	}
    	if(inputListener.escape) {
    		camera = new Camera(Camera.FOCUS_MODE, camPos, new Point3f(0, 2, 0), new Point3f(0,1,0), canvas);
    	}
    	
    	camera.review();


        for (GLObject each: runQueue) {
            each.display(camera.getCameraMatrix());
        }
    }
    
    
    //Computeable impl
    
	@Override
	public float compute(int x, int z) {
		return (float) (Math.sin(x) + Math.sin(z)) / 2;
	}

}


final class Input implements KeyListener, MouseListener {
	
	public boolean w,a,s,d,r,f,
				   up,down,right,left,
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
		switch(e.getKeyCode()) {
		case KeyEvent.VK_W: w=true; break;
		case KeyEvent.VK_A: a=true; break;
		case KeyEvent.VK_S: s=true; break;
		case KeyEvent.VK_D: d=true; break;
		case KeyEvent.VK_R: r=true; break;
		case KeyEvent.VK_F: f=true; break;
		case KeyEvent.VK_UP: up=true; break;
		case KeyEvent.VK_DOWN: down=true; break;
		case KeyEvent.VK_RIGHT: right=true; break;
		case KeyEvent.VK_LEFT: left=true; break;
		case KeyEvent.VK_ESCAPE: escape=true; break;
		}		
	}

	@Override
	public void keyReleased(KeyEvent e) {
		switch(e.getKeyCode()) {
		case KeyEvent.VK_W: w=false; break;
		case KeyEvent.VK_A: a=false; break;
		case KeyEvent.VK_S: s=false;break;
		case KeyEvent.VK_D: d=false;break;
		case KeyEvent.VK_R: r=false; break;
		case KeyEvent.VK_F: f=false; break;
		case KeyEvent.VK_UP: up=false; break;
		case KeyEvent.VK_DOWN: down=false; break;
		case KeyEvent.VK_RIGHT: right=false; break;
		case KeyEvent.VK_LEFT: left=false; break;
		case KeyEvent.VK_ESCAPE: escape=false; break;
		}
	}
	
	
	//MOUSELISTENER

	@Override
	public void mouseClicked(MouseEvent e) {
		
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

