/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * KneadEmulator.java
 *
 * Created on 07.11.2009, 13:02:57
 *
 */

package de.naoth.me.emulation;

import de.naoth.me.core.*;

//import java.awt.GraphicsConfiguration;
//import java.awt.GraphicsEnvironment;
//import java.awt.Rectangle;
//
//import java.io.FileNotFoundException;
//import java.util.logging.Level;
//import java.util.logging.Logger;
//import javax.media.j3d.*;
//import javax.vecmath.*;
import java.awt.event.MouseWheelEvent;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.media.opengl.DebugGL2;
import javax.media.opengl.GL;
import javax.media.opengl.GL2;
import javax.media.opengl.GL2ES1;
import javax.media.opengl.fixedfunc.GLLightingFunc;
import javax.media.opengl.fixedfunc.GLMatrixFunc;

import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.GLProfile;
import javax.media.opengl.awt.GLCanvas;
import javax.media.opengl.glu.GLU;
import javax.media.opengl.glu.GLUquadric;

import com.jogamp.opengl.util.FPSAnimator;;


//import com.sun.j3d.loaders.*;
import java.util.Enumeration;
import java.util.HashMap;
//import java.util.Map;
//import org.jdesktop.j3d.loaders.vrml97.VrmlLoader;
import javax.swing.JPanel;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.JOptionPane;

//import com.sun.j3d.utils.geometry.Text2D;

import java.awt.Font;

import javax.media.opengl.*;
import js.jogl.model.*;
import js.math.vector.*;

import java.util.ArrayList;
import java.util.Hashtable;


import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;

/**
 *
 * @author claas
 */
public class KneadEmulatorJOGL extends javax.media.opengl.awt.GLJPanel implements GLEventListener
{
   /** Serial version UID. */
    private static final long serialVersionUID = 1L;

    /** The GL unit (helper class). */
    private GLU glu;
    private GL2 gl;
    /** The frames per second setting. */
    private int fps = 60;

    private static float deltaZ=0;

    /** The OpenGL animator. */
    private FPSAnimator animator;

    private ParserVRML meshParser;

    private Vec3f viewPosition;
    private Vec3f viewRotation;


    public KneadEmulatorJOGL()
    {
//      GLProfile glp = GLProfile.get(GLProfile.GL2);
//      GLCapabilities caps = new GLCapabilities(glp);
//      caps.setRedBits(8);
//      caps.setBlueBits(8);
//      caps.setGreenBits(8);
//      caps.setAlphaBits(8);

      //canvas = new GLCanvas(caps);
      animator = new FPSAnimator(this, fps);
      //this.add(canvas);
      this.addGLEventListener(this);

      // Start animator (which should be a field).
      animator.start();

      meshParser = new ParserVRML();

      viewPosition = new Vec3f(0);
      viewRotation = new Vec3f(0);

//        MouseWheelListener[] allMWLs = this.getMouseWheelListeners();
//        for(int i = 0; i < allMWLs.length; i++)
//        {
//          this.removeMouseWheelListener(allMWLs[i]);
//        }
//
//        this.addMouseWheelListener
//        (
//          new MouseWheelListener()
//          {
//            public void mouseWheelMoved(MouseWheelEvent e)
//            {
//              int dR = e.getWheelRotation();
//              if(dR > 0)
//              {
//                viewPosition.z += 0.1;
//              }
//              else if(dR < 0)
//              {
//                viewPosition.z -= 0.1;
//              }
//            }
//          }
//        );





//      new JoglApplication(new HelloWorld(), "Hello World", 480, 320, false);
    }
    
    @Override
    public void init(GLAutoDrawable drawable)
    {
      String modelPath = "./3d_models";

      js.jogl.model.ObjMeshImporter importer = new js.jogl.model.ObjMeshImporter(modelPath);

      try
      {
        meshParser.parse(modelPath + "/env/Light.wrl");

        meshParser.parse(modelPath + "/env/Origin.wrl");
//        meshParser.parse(modelPath + "/env/Floor.wrl");
//
//        meshParser.parse(modelPath + "/nao/v3+/Head.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/Torso.wrl");
//
//        meshParser.parse(modelPath + "/nao/v3+/LBicep.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/LForeArm.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/LHip.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/LPelvis.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/LThigh.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/LTibia.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/LAnkle.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/LFoot.wrl");
//
//        meshParser.parse(modelPath + "/nao/v3+/RBicep.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/RForeArm.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/RHip.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/RPelvis.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/RThigh.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/RTibia.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/RAnkle.wrl");
//        meshParser.parse(modelPath + "/nao/v3+/RFoot.wrl");
      }
      catch (IOException ex)
      {
        Logger.getLogger(KneadEmulatorJOGL.class.getName()).log(Level.SEVERE, null, ex);
      }

//      ParserVRML meshParser1 = new ParserVRML(modelPath + "/env/Origin.wrl");
//      try
//      {
//        meshParser1.parse();
//      }
//      catch (IOException ex)
//      {
//        Logger.getLogger(KneadEmulatorJOGL.class.getName()).log(Level.SEVERE, null, ex);
//      }
//
//      ParserVRML meshParser2 = new ParserVRML(modelPath + "/nao/v3+/Head.wrl");
//      try
//      {
//        meshParser2.parse();
//      }
//      catch (IOException ex)
//      {
//        Logger.getLogger(KneadEmulatorJOGL.class.getName()).log(Level.SEVERE, null, ex);
//      }


			gl = drawable.getGL().getGL2();
			glu=new GLU();

//      gl.glPolygonMode(GL2.GL_FRONT_AND_BACK, GL2.GL_LINE);
//      gl.glPolygonMode(GL2.GL_FRONT_AND_BACK, GL2.GL_POINT);
      gl.glPolygonMode(GL2.GL_FRONT_AND_BACK, GL2.GL_FILL);

      gl.glEnable(GL2.GL_LIGHTING);

      for(int i = 0; i < meshParser.spotLights.size(); i++)
      {
        SpotlightD spot =  meshParser.spotLights.get(i);

        float color[] = {spot.color.x, spot.color.y, spot.color.z, 1};
        float position[] = {spot.location.x, spot.location.y, spot.location.z, 1};
        float direction[] = {spot.direction.x, spot.direction.y, spot.direction.z, 1};

        gl.glLightfv(i, GL2.GL_AMBIENT, color, 0 );
        gl.glLightfv(i, GL2.GL_DIFFUSE, color, 0 );
        gl.glLightfv(i, GL2.GL_SPECULAR, color, 0 );

        gl.glLightf(i, GL2.GL_CONSTANT_ATTENUATION,0.5f );
 
        gl.glLightfv(i, GL2.GL_POSITION,position, 0 );
        gl.glLightf(i, GL2.GL_SPOT_CUTOFF, spot.cutOffAngle);
        gl.glLightfv(i, GL2.GL_SPOT_DIRECTION, direction, 0 );
        gl.glLightf(i, GL2.GL_SPOT_EXPONENT, 50f  );

      }





//
			float ambient[]= {0.2f,0.2f,0.2f,1};
			gl.glLightModelfv(GL2.GL_LIGHT_MODEL_AMBIENT , ambient,0);

			gl.glEnable(GL2.GL_LIGHT0);
			float position[]= {-0.4f,0.5f,0.7f,1};
			gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_POSITION, position, 0);
			float intensity[]= {1,1,1,1};
			gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_DIFFUSE, intensity, 0);

			gl.glEnable(GL2.GL_LIGHT1);
			float position2[]= {0,-0.8f,0.3f,1};
			gl.glLightfv(GL2.GL_LIGHT1, GL2.GL_POSITION, position2, 0);
			float intensity2[]= {1,0,0,0};
			gl.glLightfv(GL2.GL_LIGHT1, GL2.GL_DIFFUSE, intensity2, 0);
			float specIntensity2[]= {1,1,1,1};
			gl.glLightfv(GL2.GL_LIGHT1, GL2.GL_SPECULAR, specIntensity2, 0);


			gl.glEnable(GL2.GL_COLOR_MATERIAL);
			gl.glColorMaterial(GL.GL_FRONT_AND_BACK, GL2.GL_AMBIENT_AND_DIFFUSE);
			float specColor[]= {1,1,1,1};
			gl.glMaterialfv(GL.GL_FRONT_AND_BACK,GL2.GL_SPECULAR, specColor,0);
			gl.glMaterialf(GL.GL_FRONT_AND_BACK,GL2.GL_SHININESS, 80);

			gl.glEnable(GL.GL_BLEND);
			gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);







//        // Get the OpenGL graphics context
//        GL2 gl = drawable.getGL().getGL2();
//        // Get GL Utilities after the GL context created.
//        glu = GLU.createGLU();
//        // Enable smooth shading, which blends colors nicely, and smoothes out lighting.
//        gl.glShadeModel(GLLightingFunc.GL_SMOOTH);
//        // Set background color in RGBA. Alpha: 0 (transparent) 1 (opaque)
//        gl.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//        // Setup the depth buffer and enable the depth testing
//        gl.glClearDepth(1.0f);         // clear z-buffer to the farthest
//        gl.glEnable(GL.GL_DEPTH_TEST); // enables depth testing
//        gl.glDepthFunc(GL.GL_LEQUAL);  // the type of depth test to do
//        // Do the best perspective correction
//        gl.glHint(GL2ES1.GL_PERSPECTIVE_CORRECTION_HINT, GL.GL_NICEST);

//        // Start animator (which should be a field).
//        animator.start();
//        setSize(this.getWidth(), this.getHeight());
//        this.display();
    }

    @Override
    public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height)
    {
//      gl.glMatrixMode(GL2.GL_PROJECTION);
//      gl.glLoadIdentity();
//
//      gl.glFrustum( -0.1f, 0.1f, -0.1f, 0.1f, 0.2f, 100.0f );
//      double aspect = (double)width / (double)height;
//      // Field ov view in y direction [0..180].
//      final double fovy = 30.0f;
//
//      gl.glViewport( 0, 0, width, height );
//

      // Get the OpenGL graphics context
//      GL2 gl = drawable.getGL().getGL2();
////
//      height = (height == 0) ? 1 : height;  // Prevent divide by zero
//      float aspect = (float)width / height; // Compute aspect ratio
//
      // Set view port to cover full screen
//      gl.glViewport(0, 0, width, height);

//      // Set up the projection matrix - choose perspective view
//      gl.glMatrixMode(GLMatrixFunc.GL_PROJECTION);
//      gl.glLoadIdentity();    // reset
//      // Angle of view (fovy) is 45 degrees (in the up y-direction). Based on this
//      // canvas's aspect ratio. Clipping z-near is 0.1f and z-near is 100.0f.
//      glu.gluPerspective(45.0f, aspect, 0.1f, 100.0f); // fovy, aspect, zNear, zFar

//      // Switch to the model-view transform
//      gl.glMatrixMode(GLMatrixFunc.GL_MODELVIEW);
//      gl.glLoadIdentity();    // reset
    }

    public void displayChanged(GLAutoDrawable drawable, boolean modeChanged, boolean deviceChanged)
    {
    }

    @Override
    public void display(GLAutoDrawable drawable)
    {
      // Get the OpenGL graphics context
//      gl = drawable.getGL().getGL2();
      // Clear the color and the depth buffers
//      gl.glClear(GL2.GL_COLOR_BUFFER_BIT | GL2.GL_DEPTH_BUFFER_BIT);
//      // Reset the view (x, y, z axes back to normal)
//      gl.glClear(GL.GL_COLOR_BUFFER_BIT);
//      gl.glLoadIdentity();

			gl.glClear(GL2.GL_COLOR_BUFFER_BIT | GL2.GL_DEPTH_BUFFER_BIT);


			gl.glEnable(GL2.GL_DEPTH_TEST);
			gl.glCullFace(GL2.GL_FRONT);
			gl.glEnable(GL2.GL_CULL_FACE);
//			gl.glDisable(GL2.GL_CULL_FACE);
			gl.glFrontFace(GL2.GL_CW);
//			gl.glFrontFace(GL2.GL_CCW);

			GLUquadric quadric = glu.gluNewQuadric();
			glu.gluQuadricNormals(quadric, GL2.GL_TRUE);


      TransformD origin = meshParser.namedTransforms.get("Origin");
      ArrayList<ObjectVRML> origins = origin.children;
      Vec3d transO = origin.translation;
      Vec3d rotO = origin.rotation;
      double angleO = origin.angle;


      
      viewPosition.z -= 0.001;

      gl.glTranslatef(viewPosition.x, viewPosition.y, viewPosition.z);


//      gl.glPushMatrix();

      for(int i = 0; i < origins.size(); i++)
      {
        Vec3d trans = ((TransformD) origins.get(i)).translation;
        Vec3d rot = ((TransformD) origins.get(i)).rotation;
        double angle = ((TransformD) origins.get(i)).angle  * 180 / Math.PI;
        CylinderD cylinder = (CylinderD) ((ShapeD) ((TransformD) origins.get(i)).children.get(0)).geometry;
        gl.glColor4f(0.7f,1,0.7f,1);

        if(cylinder.bottom)
        {
          gl.glLoadIdentity();
          gl.glTranslated(0, 0, cylinder.height * 5);
          gl.glRotated(angle, rot.x, rot.y, rot.z);
          gl.glTranslated(trans.x, trans.y, trans.z);
          glu.gluDisk(quadric, 0.0, cylinder.radius * 5, 6, 1);
        }

        gl.glLoadIdentity();
        gl.glRotated(angle, rot.x, rot.y, rot.z);
        gl.glTranslated(trans.x, trans.y, trans.z);
        glu.gluCylinder(quadric, cylinder.radius * 5, cylinder.radius * 5, cylinder.height * 5, 6, 1);

        if(cylinder.top)
        {
          gl.glLoadIdentity();
          gl.glRotated(angle, rot.x, rot.y, rot.z);
          gl.glTranslated(trans.x, trans.y, trans.z);
          glu.gluDisk(quadric, 0.0, cylinder.radius * 5, 6, 1);
        }

      }
//      gl.glPopMatrix();

//      gl.glLoadIdentity();
//      glu.gluLookAt(-0.5, -0.5, -2.5, 0, 0, 0, 0, 1, 0);

//      gl.glLoadIdentity();


//			gl.glLoadIdentity();
//
//      gl.glColor4f(0.7f,1,0.7f,1);
//      glu.gluSphere(quadric, 0.5f, 90, 90);
//
//			gl.glLoadIdentity();
//			gl.glTranslatef(0.2f,0,deltaZ);
//
//      gl.glColor4f(1,0.7f,0.7f,0.4f);
//      glu.gluSphere(quadric, 0.4f, 90, 90);




//      gl.glTranslatef(-0.5f, -0.5f, 0.0f);

      // ----- Your OpenGL rendering code here -----
      // ......
//      gl.glBegin(GL.GL_TRIANGLES);
//      gl.glColor3f(1, 0, 0);
//      gl.glVertex2d(-0.5, -0.5);
//      gl.glColor3f(0, 1, 0);
//      gl.glVertex2d(0, 0.5);
//      gl.glColor3f(0, 0, 1);
//      gl.glVertex2d(0.5, -0.5);
//      gl.glEnd();
//
//      // ----- Render a triangle -----
////      gl.glTranslatef(0.5f, 0.5f, -2.0f); // translate left and into the screen
//      gl.glBegin(GL.GL_TRIANGLES); // draw using triangles
//      // Define groups of 3 vertices in counter-clockwise (CCW) order
//      gl.glColor3f(1, 0, 0);
//      gl.glVertex3d(0.0f, 1.0f, 2.0f);
//      gl.glVertex3d(-1.0f, -1.0f, 2.0f);
//      gl.glVertex3d(1.0f, -1.0f, 2.0f);
//      gl.glEnd();

//      // ----- Render a quad -----
//      // translate right, relative to the previous translation
////      gl.glTranslatef(3.0f, 0.0f, 0.0f);
//      gl.glBegin(GL2.GL_QUADS); // draw using quads
//      // Define groups of 4 vertices in CCW order
//      gl.glVertex3f(-1.0f, 1.0f, 0.0f);
//      gl.glVertex3f(1.0f, 1.0f, 0.0f);
//      gl.glVertex3f(1.0f, -1.0f, 0.0f);
//      gl.glVertex3f(-1.0f, -1.0f, 0.0f);
//      gl.glEnd();
    }

    @Override
    public void dispose(GLAutoDrawable drawable)
    {

    }
//    private VirtualUniverse universe = null;
//    private BoundingSphere globalBounds = null;
//    private Locale locale = null;
//    private Map<String, TransformGroup> namedObjects;
//    private Map<String, BranchGroup> namedGroupObjects;
//
//    private TransformGroup coordinationTG = null;
//    private Canvas3D canvas_nao;
//
//    private Vector3d worldMiddle = null;
//    private Vector3d headOrigin = null;
//    private Vector3d lShoulderOrigin = null;
//    private Vector3d rShoulderOrigin = null;
//    private Vector3d elbowOrigin = null;
//    private Vector3d lHipOrigin = null;
//    private Vector3d rHipOrigin = null;
//    private Vector3d kneeOrigin = null;
//    private Vector3d ankleOrigin = null;
//    private Vector3d textOrigin = null;
//
//    private JointConfiguration config = null;;
//    private Shape3D infoText = null;
//    private OrbitBehavior orbit = null;
//
//    private Transform3D t3d = new Transform3D();
//
//    /** Creates new form KneadEmulator */
//    public KneadEmulatorJOGL() throws java.lang.UnsatisfiedLinkError
//    {
//      try
//      {
//        globalBounds = new BoundingSphere(new Point3d(0, 0, 0), Double.MAX_VALUE);
//        locale = null;
//        coordinationTG = new TransformGroup();
//
//        worldMiddle = new Vector3d(0.0, 0.0, 0.0);
//        headOrigin = new Vector3d(0.0, 0.0, 0.125);
//        lShoulderOrigin = new Vector3d(0.0, 0.1, 0.1);
//        rShoulderOrigin = new Vector3d(0.0, -0.1, 0.1);
//        elbowOrigin = new Vector3d(0.1, 0.0, 0.0);
//        lHipOrigin = new Vector3d(0.005, 0.05, -0.085);
//        rHipOrigin = new Vector3d(0.005, -0.05, -0.085);
//        kneeOrigin = new Vector3d(0.0, 0.0, -0.1);
//        ankleOrigin = new Vector3d(0.0, 0.0, -0.1);
//        textOrigin = new Vector3d(1.4460, -0.21, 0.16);
//
//        GraphicsConfigTemplate3D template = new GraphicsConfigTemplate3D();
//
//        GraphicsConfiguration gconf = GraphicsEnvironment.getLocalGraphicsEnvironment()
//          .getDefaultScreenDevice().getBestConfiguration(template);
//
//        canvas_nao = new Canvas3D(gconf);
//
//        initComponents();
//
//        this.add(canvas_nao);
//        canvas_nao.setSize(570,453);
//
//        createUniverse();
//        createViewBranch(canvas_nao, new Vector3f(2, 0, 0));
//        createSceneGraphic();
//      }
//      catch(java.lang.UnsatisfiedLinkError e)
//      {
//        JOptionPane.showMessageDialog
//        (
//          this,
//          "Java 3D is not installed!\nVisit https://java3d.dev.java.net\n" + e.toString(),
//          "Java 3D is not installed",
//          JOptionPane.ERROR_MESSAGE
//        );
//        throw e;
//      }
////      catch (Exception e)
////      {
////        JOptionPane.showMessageDialog
////        (
////          this,
////          "Can not initialize Java 3D\n",
////          e.toString(),
////          JOptionPane.ERROR_MESSAGE
////        );
////      }
//    }
//
//    public void setConfig(JointConfiguration newConfig)
//    {
//      if(newConfig == null)
//      {
//        config = new JointConfiguration(new JointDefaultConfiguration());
//      }
//      else
//      {
//        config = newConfig;
//      }
//    }
//
//    public void destroy()
//    {
//        universe.removeAllLocales();
//    }
//
//    private void createUniverse()
//    {
//        universe = new VirtualUniverse();
//        locale = new Locale(universe);
//    }
//
//    private void createSceneGraphic()
//    {
//      namedObjects = new HashMap<String, TransformGroup>();
//      namedGroupObjects = new HashMap<String, BranchGroup>();
//      Loader ldr = new VrmlLoader(Loader.LOAD_ALL);
//      String modelPath = "./3d_models";
//
//      String[] naofiles = new String[]
//                          {
//                            "Head",
//                            "Chest",
//                            "RHipYawPitch",
//                            "RHipRoll",
//                            "RThigh",
//                            "RTibia",
//                            "RAnkle",
//                            "RFoot",
//                            "LHipYawPitch",
//                            "LHipRoll",
//                            "LThigh",
//                            "LTibia",
//                            "LAnkle",
//                            "LFoot",
//                            "RUpperArm",
//                            "RLowerArm",
//                            "LUpperArm",
//                            "LLowerArm"
//                          };
//
//      coordinationTG.setCapability(TransformGroup.ALLOW_CHILDREN_EXTEND);
//      coordinationTG.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
//      coordinationTG.setCapability(TransformGroup.ENABLE_PICK_REPORTING);
//      BranchGroup coordinationBG = new BranchGroup();
//      coordinationBG.addChild(coordinationTG);
//      locale.addBranchGraph(coordinationBG);
//
//			//load all nao parts
//      BranchGroup nao = new BranchGroup();
//      for (int i = 0; i < naofiles.length; i++)
//      {
//        BranchGroup bg = load(modelPath + "/nao/v3/" + naofiles[i] + ".wrl", ldr);
//        namedGroupObjects.put(naofiles[i], bg);
//      }
//
////      Appearance app = new Appearance();
////      Color3f lightGray = new Color3f(0.8f, 0.8f, 0.8f);
////      ColoringAttributes ca = new ColoringAttributes(lightGray, ColoringAttributes.NICEST);
////      app.setColoringAttributes(ca);
//////      cameraLine.setCapability(LineArray.ALLOW_COORDINATE_WRITE);
////      Shape3D plShape = new Shape3D(cameraLine, app);
//
//      //group all directly connected parts with chest
//      TransformGroup tg = namedObjects.get("Chest");
//
//      tg.addChild(namedGroupObjects.get("Head"));
//      tg.addChild(namedGroupObjects.get("RHipYawPitch"));
//      tg.addChild(namedGroupObjects.get("LHipYawPitch"));
//      tg.addChild(namedGroupObjects.get("RUpperArm"));
//      tg.addChild(namedGroupObjects.get("LUpperArm"));
//
//      //group all directly connected parts with HipYawPitch right side ,...
//      tg = namedObjects.get("RHipYawPitch");
//      tg.addChild(namedGroupObjects.get("RHipRoll"));
//      tg = namedObjects.get("RHipRoll");
//      tg.addChild(namedGroupObjects.get("RThigh"));
//      tg = namedObjects.get("RThigh");
//      tg.addChild(namedGroupObjects.get("RTibia"));
//      tg = namedObjects.get("RTibia");
//      tg.addChild(namedGroupObjects.get("RAnkle"));
//      tg = namedObjects.get("RAnkle");
//      tg.addChild(namedGroupObjects.get("RFoot"));
//
//      //group all directly connected parts with HipYawPitch left side ,...
//      tg = namedObjects.get("LHipYawPitch");
//      tg.addChild(namedGroupObjects.get("LHipRoll"));
//      tg = namedObjects.get("LHipRoll");
//      tg.addChild(namedGroupObjects.get("LThigh"));
//      tg = namedObjects.get("LThigh");
//      tg.addChild(namedGroupObjects.get("LTibia"));
//      tg = namedObjects.get("LTibia");
//      tg.addChild(namedGroupObjects.get("LAnkle"));
//      tg = namedObjects.get("LAnkle");
//      tg.addChild(namedGroupObjects.get("LFoot"));
//
//      //group all directly connected parts with UpperArm right side
//      tg = namedObjects.get("RUpperArm");
//      tg.addChild(namedGroupObjects.get("RLowerArm"));
//
//      //group all directly connected parts with UpperArm left side
//      tg = namedObjects.get("LUpperArm");
//      tg.addChild(namedGroupObjects.get("LLowerArm"));
//
//      //add chest and all with it groupoed  parts to nao object
//      nao.addChild(namedGroupObjects.get("Chest"));
////      AmbientLight lightA = new AmbientLight();
////      lightA.setInfluencingBounds(new BoundingSphere());
////      nao.addChild(lightA);
//
//      MousePickingBehavior pick = new MousePickingBehavior(canvas_nao, nao, new BoundingSphere(), this);
//      nao.addChild(pick);
//
//      infoText = new Text2D("nothing selected", new Color3f(1f, 1f, 0f), "Serif", 20, Font.BOLD);
//      tg = new TransformGroup();
//      tg.addChild(infoText);
//      tg.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
//      tg.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
//      tg.setCapability(Shape3D.ALLOW_APPEARANCE_READ);
//      tg.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
//
//      namedObjects.put("2D Info", tg);
//
//      Transform3D t3dText = new Transform3D();
//
//      t3d = new Transform3D();
//      t3d.rotX(Math.PI / 2);
//      tg.getTransform(t3dText);
//      t3d.mul(t3dText);
//      t3dText = new Transform3D();
//      t3dText.rotZ(Math.PI / 2);
//      t3dText.mul(t3d);
//      t3dText.setScale(0.3);
//      t3d = new Transform3D();
//      t3dText.setTranslation(textOrigin);
//      tg.setTransform(t3dText);
//
//      nao.addChild(tg);
//
//
//      DirectionalLight lightD1 = new DirectionalLight();
//      lightD1.setInfluencingBounds(new BoundingSphere());
//      nao.addChild(lightD1);
//      coordinationTG.addChild(nao);
//
//      BranchGroup light = load(modelPath + "/env/Light.wrl", ldr);
//      locale.addBranchGraph(light);
//
//      setConfig(null);
//      initJoints();
//
//    }
//
//    public void setIgnoreInput(Boolean ignore)
//    {
//      orbit.setIgnoreInput(ignore);
//    }
//
//    public BranchGroup getBranchGroup(String name)
//    {
//      return namedGroupObjects.get(name);
//    }
//
//    public void setInfoText(String text)
//    {
//      ((Text2D) infoText).setString(text);
//    }
//
//    public void textRotX(double angle)
//    {
//      if(namedObjects != null && namedObjects.containsKey("2D Info"))
//      {
//        Transform3D toRot = new Transform3D();
//        TransformGroup targetTG =  namedObjects.get("2D Info");
//
//        targetTG.getTransform(t3d);
//        toRot.rotX(angle);
//        toRot.mul(t3d);
//        targetTG.setTransform(toRot);
//      }
//    }
//
//    public void textRotY(double angle)
//    {
//      if(namedObjects != null && namedObjects.containsKey("2D Info"))
//      {
//        Transform3D toRot = new Transform3D();
//        TransformGroup targetTG =  namedObjects.get("2D Info");
//
//        targetTG.getTransform(t3d);
//        toRot.rotY(angle);
//        toRot.mul(t3d);
//        targetTG.setTransform(toRot);
//      }
//    }
//    public void textRotZ(double angle)
//    {
//      if(namedObjects != null && namedObjects.containsKey("2D Info"))
//      {
//        Transform3D toRot = new Transform3D();
//        TransformGroup targetTG =  namedObjects.get("2D Info");
//
//        targetTG.getTransform(t3d);
//        toRot.rotZ(angle);
//        toRot.mul(t3d);
//        targetTG.setTransform(toRot);
//      }
//    }
//
//    public void textMove(Vector3d theMove)
//    {
//      if(namedObjects != null && namedObjects.containsKey("2D Info"))
//      {
//        Transform3D toMove = new Transform3D();
//        TransformGroup targetTG =  namedObjects.get("2D Info");
//        targetTG.getTransform(t3d);
//        t3d.setScale(1.0);
//        toMove.setTranslation(theMove);
//        t3d.mul(toMove);
//        t3d.setScale(0.3);
//        targetTG.setTransform(t3d);
//      }
//    }
//
//
//    // load vrml file and return the contents BranchGroup
//    // and add content as TranformGroup to namedObjects
//    private BranchGroup load(String filename, Loader ldr)
//    {
//      BranchGroup bg = null;
//      try
//      {
//        Scene vrml_scene = ldr.load(filename);
//        bg = vrml_scene.getSceneGroup();
//        java.util.Hashtable ht = vrml_scene.getNamedObjects();
//        if (ht != null)
//        {
//          for (Enumeration e = ht.keys(); e.hasMoreElements();)
//          {
//            String name = (String) e.nextElement();
//            Object to = ht.get(name);
//            if ( TransformGroup.class.isInstance(to) )
//            {
//              TransformGroup tg = (TransformGroup) to;
//              tg.setName(name);
//              tg.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
//              tg.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
//              tg.setCapability(TransformGroup.ENABLE_PICK_REPORTING);
//              tg.setCapability(Shape3D.ALLOW_APPEARANCE_READ);
//              tg.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
//              namedObjects.put(name, tg);
//            }
//          }
//        }
//      }
//
//
//      catch (FileNotFoundException ex)
//      {
//        Logger.getLogger(KneadEmulatorJOGL.class.getName()).log(Level.SEVERE, null, ex);
//      }      catch (IncorrectFormatException ex)
//      {
//        Logger.getLogger(KneadEmulatorJOGL.class.getName()).log(Level.SEVERE, null, ex);
//      }      catch (ParsingErrorException ex)
//      {
//        Logger.getLogger(KneadEmulatorJOGL.class.getName()).log(Level.SEVERE, null, ex);
//      }
//      return bg;
//    }
//
//    private void createViewBranch(Canvas3D canvas, Vector3f homeViewPos)
//    {
//      BranchGroup viewBranch = new BranchGroup();
//
//      TransformGroup viewTG = new TransformGroup();
//      viewTG.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
//
//      ViewPlatform vp = new ViewPlatform();
//
//      View view = new View();
//      view.setPhysicalBody(new PhysicalBody());
//      view.setPhysicalEnvironment(new PhysicalEnvironment());
//
//      view.attachViewPlatform(vp);
//      view.addCanvas3D(canvas);
//
//      orbit = new OrbitBehavior(canvas, viewTG, view, this);
//      orbit.setSchedulingBounds(globalBounds);
//      orbit.setClippingEnabled(false);
//
//      orbit.setViewingTransform(new Point3d(homeViewPos), new Point3d(0,0,0), new Vector3d(0,0,1), new Point3d(0,0,0));
//
//      viewTG.addChild(vp);
//      viewTG.addChild(orbit);
//
//      viewBranch.addChild(viewTG);
//      locale.addBranchGraph(viewBranch);
//    }
//
//    private void initJoints()
//    {
//      setHead(this.config.getJoint("HeadPitch").getRadiant(), this.config.getJoint("HeadYaw").getRadiant());
//
//      setShoulder("L", this.config.getJoint("LShoulderPitch").getRadiant(), this.config.getJoint("LShoulderRoll").getRadiant());
//      setShoulder("R", this.config.getJoint("RShoulderPitch").getRadiant(), this.config.getJoint("RShoulderRoll").getRadiant());
//
//      setElbow("L", this.config.getJoint("LElbowYaw").getRadiant(), this.config.getJoint("LElbowRoll").getRadiant());
//      setElbow("R", this.config.getJoint("RElbowYaw").getRadiant(), this.config.getJoint("RElbowRoll").getRadiant());
//
//      setAnkleRoll("L", this.config.getJoint("LAnkleRoll").getRadiant());
//      setAnklePitch("L", this.config.getJoint("LAnklePitch").getRadiant());
//      setKneePitch("L", this.config.getJoint("LKneePitch").getRadiant());
//      setHipPitch("L", this.config.getJoint("LHipPitch").getRadiant());
//      setHipRoll("L", this.config.getJoint("LHipRoll").getRadiant());
//
//      setAnkleRoll("R", this.config.getJoint("RAnkleRoll").getRadiant());
//      setAnklePitch("R", this.config.getJoint("RAnklePitch").getRadiant());
//      setKneePitch("R", this.config.getJoint("RKneePitch").getRadiant());
//      setHipPitch("R", this.config.getJoint("RHipPitch").getRadiant());
//      setHipRoll("R", this.config.getJoint("RHipRoll").getRadiant());
//
//      setHipYawPitch(this.config.getJoint("LHipYawPitch").getRadiant());
//    }
//
//    public void propertyChange(PropertyChangeEvent event)
//    {
//      if(event.getPropertyName().equals("selectedKeyFrame") )
//      {
//          if(event.getNewValue() instanceof KeyFrame)
//          {
//            KeyFrame newKeyFrame = (KeyFrame) event.getNewValue();
//            setConfig(newKeyFrame.getJointConfiguration());
//          }
//          else if(event.getNewValue() == null)
//          {
//            setConfig(null);
//          }
//          initJoints();
//      }
//
//      if(event.getPropertyName().equals("jointValueChanged"))
//      {
//        Joint joint = (Joint) event.getNewValue();
//
//        if(joint.getId().equals("HeadPitch"))
//        {
//          setHead(joint.getRadiant(), config.getJoint("HeadYaw").getRadiant());
//        }
//        if(joint.getId().equals("HeadYaw"))
//        {
//          setHead(config.getJoint("HeadPitch").getRadiant(), joint.getRadiant());
//        }
//
//        if(joint.getId().equals("LShoulderPitch"))
//        {
//          setShoulder("L", joint.getRadiant(), config.getJoint("LShoulderRoll").getRadiant());
//        }
//        if(joint.getId().equals("RShoulderPitch"))
//        {
//          setShoulder("R", joint.getRadiant(), config.getJoint("RShoulderRoll").getRadiant());
//        }
//
//        if(joint.getId().equals("LShoulderRoll"))
//        {
//          setShoulder("L", config.getJoint("LShoulderPitch").getRadiant(), joint.getRadiant());
//        }
//        if(joint.getId().equals("RShoulderRoll"))
//        {
//          setShoulder("R", config.getJoint("RShoulderPitch").getRadiant(), joint.getRadiant());
//        }
//
//        if(joint.getId().equals("LElbowRoll"))
//        {
//          setElbow("L", config.getJoint("LElbowYaw").getRadiant(), joint.getRadiant());
//        }
//        if(joint.getId().equals("RElbowRoll"))
//        {
//          setElbow("R", config.getJoint("RElbowYaw").getRadiant(), joint.getRadiant());
//        }
//
//        if(joint.getId().equals("LElbowYaw"))
//        {
//          setElbow("L", joint.getRadiant(), config.getJoint("LElbowRoll").getRadiant());
//        }
//        if(joint.getId().equals("RElbowYaw"))
//        {
//          setElbow("R", joint.getRadiant(), config.getJoint("RElbowRoll").getRadiant());
//        }
//
//        if(joint.getId().equals("LHipYawPitch"))
//        {
//          setHipYawPitch(joint.getRadiant());
//        }
//
//        if(joint.getId().equals("RHipYawPitch"))
//        {
//          setHipYawPitch(joint.getRadiant());
//        }
//
//        if(joint.getId().equals("LHipRoll"))
//        {
//          setHipRoll("L", joint.getRadiant());
//        }
//        if(joint.getId().equals("RHipRoll"))
//        {
//          setHipRoll("R", joint.getRadiant());
//        }
//
//        if(joint.getId().equals("LHipPitch"))
//        {
//          setHipPitch("L", joint.getRadiant());
//        }
//        if(joint.getId().equals("RHipPitch"))
//        {
//          setHipPitch("R", joint.getRadiant());
//        }
//
//        if(joint.getId().equals("LKneePitch"))
//        {
//          setKneePitch("L", joint.getRadiant());
//        }
//        if(joint.getId().equals("RKneePitch"))
//        {
//          setKneePitch("R", joint.getRadiant());
//        }
//
//        if(joint.getId().equals("LAnklePitch"))
//        {
//          setAnklePitch("L", joint.getRadiant());
//        }
//        if(joint.getId().equals("RAnklePitch"))
//        {
//          setAnklePitch("R", joint.getRadiant());
//        }
//
//        if(joint.getId().equals("LAnkleRoll"))
//        {
//          setAnkleRoll("L", joint.getRadiant());
//        }
//        if(joint.getId().equals("RAnkleRoll"))
//        {
//          setAnkleRoll("R", joint.getRadiant());
//        }
//
//      }
//    }//end propertyChange
//
//    public void setHeadJoints( double pitch, double yaw)
//    {
//      Joint joint = config.getJoint("HeadPitch");
//      pitch += joint.getRadiant();
//      double angleD = getCheckedJointValues(joint, pitch * 180/ Math.PI);
//      pitch = angleD / 180 * Math.PI;
//
//      config.setJointValue("HeadPitch", angleD);
//
//      joint = config.getJoint("HeadYaw");
//      yaw += joint.getRadiant();
//      angleD = getCheckedJointValues(joint, yaw * 180/ Math.PI);
//      yaw = angleD / 180 * Math.PI;
//
//      config.setJointValue("HeadYaw", angleD);
//    }
//
//    public void setHead( double pitch, double yaw)
//    {
//      Transform3D trans = new Transform3D();
//      Transform3D transRot = new Transform3D();
//      TransformGroup head = namedObjects.get("Head");
//
//      head.getTransform(trans);
//      trans.setTranslation(worldMiddle);
//      trans.rotY(pitch);
//      transRot.rotZ(yaw);
//      transRot.mul(trans);
//      transRot.setTranslation(headOrigin);
//      head.setTransform(transRot);
//    }
//
//    public void setShoulderJoints(String side, double pitch, double roll)
//    {
//      Joint joint = config.getJoint(side + "ShoulderPitch");
//      pitch += joint.getRadiant();
//      double angleD = getCheckedJointValues(joint, pitch * 180/ Math.PI);
//      pitch = angleD / 180 * Math.PI;
//
//      config.setJointValue(side + "ShoulderPitch", angleD);
//
//      joint = config.getJoint(side + "ShoulderRoll");
//      roll += joint.getRadiant();
//      angleD = getCheckedJointValues(joint, roll * 180/ Math.PI);
//      roll = angleD / 180 * Math.PI;
//
//      config.setJointValue(side + "ShoulderRoll", angleD);
//    }
//
//    public void setShoulder(String side, double pitch, double roll)
//    {
//      Transform3D trans = new Transform3D();
//      Transform3D transRot = new Transform3D();
//      TransformGroup shoulder = namedObjects.get(side + "UpperArm");
//
//      shoulder.getTransform(trans);
//      trans.setTranslation(worldMiddle);
//      trans.rotZ(roll);
//      transRot.rotY(pitch);
//      transRot.mul(trans);
//      if(side.equals("L"))
//      {
//        transRot.setTranslation(lShoulderOrigin);
//      }
//      else
//      {
//        transRot.setTranslation(rShoulderOrigin);
//      }
//      shoulder.setTransform(transRot);
//    }
//
//    public void setElbowJoints(String side, double roll, double yaw)
//    {
//      Joint joint = config.getJoint(side + "ElbowRoll");
//      roll += joint.getRadiant();
//      double angleD = getCheckedJointValues(joint, roll * 180/ Math.PI);
//      roll = angleD / 180 * Math.PI;
//
//      config.setJointValue(side + "ElbowRoll", angleD);
//
//      joint = config.getJoint(side + "ElbowYaw");
//      yaw += joint.getRadiant();
//      angleD = getCheckedJointValues(joint, yaw * 180/ Math.PI);
//      yaw = angleD / 180 * Math.PI;
//
//      config.setJointValue(side + "ElbowYaw", angleD);
//    }
//
//    public void setElbow(String side, double roll, double yaw)
//    {
//      Transform3D trans = new Transform3D();
//      Transform3D transRot = new Transform3D();
//      TransformGroup arm = namedObjects.get(side + "LowerArm");
//
//      arm.getTransform(trans);
//      trans.setTranslation(worldMiddle);
//      trans.rotZ(yaw);
//      transRot.rotX(roll);
//      transRot.mul(trans);
//      transRot.setTranslation(elbowOrigin);
//      arm.setTransform(transRot);
//    }
//
//    public void setAnkleRollJoint(String side, double angle)
//    {
//      Joint joint = config.getJoint(side + "AnkleRoll");
//      angle += joint.getRadiant();
//      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
//      angle = angleD / 180 * Math.PI;
//
//      config.setJointValue(side + "AnkleRoll", angle / Math.PI * 180);
//    }
//
//    public void setAnkleRoll(String side, double angle)
//    {
//      Transform3D footTrans = new Transform3D();
//      TransformGroup footLeft = namedObjects.get(side + "Foot");
//
//      footLeft.getTransform(footTrans);
//      footTrans.setTranslation(worldMiddle);
//      footTrans.rotX(angle);
//      footLeft.setTransform(footTrans);
//    }
//
//    public void setAnklePitchJoint(String side, double angle)
//    {
//      Joint joint = config.getJoint(side + "AnklePitch");
//      angle += joint.getRadiant();
//      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
//      angle = angleD / 180 * Math.PI;
//
//      config.setJointValue(side + "AnklePitch", angle / Math.PI * 180);
//    }
//
//    public void setAnklePitch(String side, double angle)
//    {
//      Transform3D ankleTrans = new Transform3D();
//      TransformGroup ankleLeft;
//
//      ankleLeft= namedObjects.get(side + "Ankle");
//      ankleLeft.getTransform(ankleTrans);
//      ankleTrans.setTranslation(worldMiddle);
//      ankleTrans.rotY(angle);
//      ankleTrans.setTranslation(ankleOrigin);
//      ankleLeft.setTransform(ankleTrans);
//    }
//
//    public void setKneePitchJoint(String side, double angle)
//    {
//      Joint joint = config.getJoint(side + "KneePitch");
//      angle += joint.getRadiant();
//      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
//      angle = angleD / 180 * Math.PI;
//
//      config.setJointValue(side + "KneePitch", angle / Math.PI * 180);
//    }
//
//    public void setKneePitch(String side, double angle)
//    {
//      Transform3D kneeTrans = new Transform3D();
//      TransformGroup kneeLeft = namedObjects.get(side + "Tibia");
//
//      kneeLeft.getTransform(kneeTrans);
//      kneeTrans.setTranslation(worldMiddle);
//      kneeTrans.rotY(angle);
//      kneeTrans.setTranslation(kneeOrigin);
//      kneeLeft.setTransform(kneeTrans);
//    }
//
//    public void setHipPitchJoint(String side, double angle)
//    {
//      Joint joint = config.getJoint(side + "HipPitch");
//      angle += joint.getRadiant();
//      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
//      angle = angleD / 180 * Math.PI;
//
//      config.setJointValue(side + "HipPitch", angle / Math.PI * 180);
//    }
//
//    public void setHipPitch(String side, double angle)
//    {
//      Transform3D hipTrans = new Transform3D();
//      TransformGroup hipLeft = namedObjects.get(side + "Thigh");
//
//      hipLeft.getTransform(hipTrans);
//      hipTrans.setTranslation(worldMiddle);
//      hipTrans.rotY(angle);
//      hipLeft.setTransform(hipTrans);
//    }
//
//    public void setHipRollJoint(String side, double angle)
//    {
//      Joint joint = config.getJoint(side + "HipRoll");
//      angle += joint.getRadiant();
//      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
//      angle = angleD / 180 * Math.PI;
//
//      config.setJointValue(side + "HipRoll", angle / Math.PI * 180);
//    }
//
//    public void setHipRoll(String side, double angle)
//    {
//      Transform3D hipTrans = new Transform3D();
//      TransformGroup hipLeft = namedObjects.get(side + "HipRoll");
//
//      hipLeft.getTransform(hipTrans);
//      hipTrans.setTranslation(worldMiddle);
//      hipTrans.rotX(angle);
//      hipLeft.setTransform(hipTrans);
//    }
//
//    public void setHipYawPitchJoints(double angle)
//    {
//      Joint joint = config.getJoint("LHipYawPitch");
//      angle += joint.getRadiant();
//      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
//      angle = angleD / 180 * Math.PI;
//
//      config.setJointValue("LHipYawPitch", angle / Math.PI * 180);
//
//      config.setJointValue("RHipYawPitch", angle / Math.PI * 180);
//      joint = config.getJoint("RHipYawPitch");
//    }
//
//    public void setHipYawPitch(double angle)
//    {
//      Transform3D hipTrans = new Transform3D();
//      TransformGroup hip = namedObjects.get("LHipYawPitch");
//
//      hip.getTransform(hipTrans);
//      hipTrans.setTranslation(worldMiddle);
//      AxisAngle4d axis = new AxisAngle4d(0, 1, -1, angle);
//      hipTrans.setRotation(axis);
//      hipTrans.setTranslation(lHipOrigin);
//      hip.setTransform(hipTrans);
//
//      hip = namedObjects.get("RHipYawPitch");
//
//      hip.getTransform(hipTrans);
//      hipTrans.setTranslation(worldMiddle);
//      axis = new AxisAngle4d(0, 1, 1, angle);
//      hipTrans.setRotation(axis);
//      hipTrans.setTranslation(rHipOrigin);
//      hip.setTransform(hipTrans);
//    }
//
//    private double getCheckedJointValues(Joint joint, double angle)
//    {
//      if(angle > joint.getMaxValue())
//      {
//        return joint.getMaxValue();
//      }
//      if(angle < joint.getMinValue())
//      {
//        return joint.getMinValue();
//      }
//      return angle;
//    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    setPreferredSize(new java.awt.Dimension(570, 453));
    addMouseWheelListener(new java.awt.event.MouseWheelListener() {
      public void mouseWheelMoved(java.awt.event.MouseWheelEvent evt) {
        formMouseWheelMoved(evt);
      }
    });
    addMouseListener(new java.awt.event.MouseAdapter() {
      public void mouseClicked(java.awt.event.MouseEvent evt) {
        formMouseClicked(evt);
      }
    });
    addComponentListener(new java.awt.event.ComponentAdapter() {
      public void componentResized(java.awt.event.ComponentEvent evt) {
        formComponentResized(evt);
      }
    });

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
    this.setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGap(0, 570, Short.MAX_VALUE)
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGap(0, 453, Short.MAX_VALUE)
    );
  }// </editor-fold>//GEN-END:initComponents

    private void formComponentResized(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_formComponentResized
//      Rectangle bounds = this.getBounds();
////      bounds.height -= 80;
//      canvas_nao.setBounds(bounds);
//      canvas_nao.repaint();
    }//GEN-LAST:event_formComponentResized

    private void formMouseWheelMoved(java.awt.event.MouseWheelEvent evt) {//GEN-FIRST:event_formMouseWheelMoved

    int dR = evt.getWheelRotation();
    if(dR > 0)
    {
      viewPosition.z += 0.1;
    }
    else if(dR < 0)
    {
      viewPosition.z -= 0.1;
    }

    }//GEN-LAST:event_formMouseWheelMoved

    private void formMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_formMouseClicked
       viewPosition.z += 0.1;
    }//GEN-LAST:event_formMouseClicked


  // Variables declaration - do not modify//GEN-BEGIN:variables
  public final javax.swing.JPopupMenu jPopupMenuSelect = new javax.swing.JPopupMenu();
  // End of variables declaration//GEN-END:variables

}
