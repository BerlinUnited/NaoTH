/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.drawings3d;

import org.jogamp.java3d.BranchGroup;

import org.jogamp.java3d.utils.universe.SimpleUniverse;

import org.jogamp.java3d.View;
import org.jogamp.java3d.AmbientLight;
import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.Background;
import org.jogamp.java3d.BoundingSphere;
import org.jogamp.java3d.ColoringAttributes;
import org.jogamp.java3d.LineArray;
import org.jogamp.java3d.PolygonAttributes;
import org.jogamp.java3d.QuadArray;
import org.jogamp.java3d.Shape3D;
import org.jogamp.java3d.utils.geometry.Primitive;

import org.jogamp.java3d.exp.swing.JCanvas3D;

import org.jogamp.vecmath.Color3f;
import org.jogamp.vecmath.Point3d;
import org.jogamp.vecmath.Point3f;


/*
 * Singletion class:
 * the java 3D can have one VirtualUniverse only
 */
public class VirtualWorld
{

  //private final VirtualUniverse universe;
  private final SimpleUniverse universe;
 
  private final BranchGroup root;
  
  // fixed objects
  private final BranchGroup field;  
  private final BranchGroup fieldLab;

  private final BranchGroup coordinates;
  
  //private final BoundingSphere globalBounds = new BoundingSphere(new Point3d(0, 0, 0), Double.MAX_VALUE);

  public VirtualWorld(JCanvas3D canvas)
  {
    universe = new SimpleUniverse(canvas.getOffscreenCanvas3D());
    
    universe.getViewingPlatform().setNominalViewingTransform();
    universe.getViewer().getView().setMinimumFrameCycleTime(30);
    
    universe.getViewer().getView().setTransparencySortingPolicy(View.TRANSPARENCY_SORT_GEOMETRY);
    
    
    root = new BranchGroup();
    
    // setup the environment
    
    // background
    BoundingSphere sphere = new BoundingSphere(new Point3d(0,0,0), 100000);
    Background background = new Background(new Color3f(0.9f,0.9f,0.9f));
    background.setApplicationBounds(sphere);
    root.addChild(background);

    
    // light
    // Set up the ambient light
    /*
    Color3f ambientColor = new Color3f(0.3f, 0.3f, 0.3f);
    AmbientLight ambientLightNode = new AmbientLight(ambientColor);
    ambientLightNode.setInfluencingBounds(sphere);
    ambientLightNode.setEnable(true);
    root.addChild(ambientLightNode);
    */
    
    root.addChild(new Entity("Light"));

    field = new BranchGroup();
    fieldLab = new BranchGroup();

    // load entities
    field.addChild(new Entity("Field"));
    field.setCapability(BranchGroup.ALLOW_DETACH);
    //enableField(true);
    
    fieldLab.addChild(new Entity("FieldLab"));
    fieldLab.setCapability(BranchGroup.ALLOW_DETACH);
    
    coordinates = createCoordinateSystem();
    coordinates.setCapability(BranchGroup.ALLOW_DETACH);
    
    
    // orbiting platform
    BetterOrbitBehavior orbit = new BetterOrbitBehavior(canvas);
    universe.getViewingPlatform().setViewPlatformBehavior(orbit);
    
    orbit.setSchedulingBounds(new BoundingSphere(new Point3d(0.0, 0.0, 0.0), 100.0));
    orbit.setTranslateEnable(true);
    orbit.setReverseRotate(true);
    orbit.setEnable(true);
    
    
    universe.addBranchGraph(root);
  }
  
  public void cleanup() {
      //universe.cleanup();
      universe.removeAllLocales();
      Primitive.clearGeometryCache();
  }
  
  // TODO
  private final BranchGroup createScene() 
  {
      return null;
  }
  
  public final BranchGroup createCoordinateSystem() 
  {
    BranchGroup objRoot = new BranchGroup();
     
    int n = 40;
    float limit = 10.0f;
    float step = 2*limit/(float)n;
    
    // grid
    Color3f colorG = new Color3f(0.4f, 0.7f, 0.9f);
    Appearance lineAppearance = new Appearance();
    lineAppearance.setColoringAttributes(new ColoringAttributes(colorG, ColoringAttributes.SHADE_FLAT));
    
    // Create X axis
    LineArray axisXLines = new LineArray((n+1)*4,LineArray.COORDINATES);
    for(int i = 0; i <= n; ++i) {
        axisXLines.setCoordinate(4*i+0, new Point3f(i*step-limit,-limit, 0.006f));
        axisXLines.setCoordinate(4*i+1, new Point3f(i*step-limit, limit, 0.006f));
        
        axisXLines.setCoordinate(4*i+2, new Point3f(-limit, i*step-limit, 0.006f));
        axisXLines.setCoordinate(4*i+3, new Point3f( limit, i*step-limit, 0.006f));
    }
    objRoot.addChild(new Shape3D(axisXLines, lineAppearance));
    
    // ground plane
    Color3f color = new Color3f(0.3f, 0.4f, 0.5f);
    Appearance planeAppearance = new Appearance();
    planeAppearance.setColoringAttributes(new ColoringAttributes(color, ColoringAttributes.SHADE_FLAT));
    planeAppearance.setPolygonAttributes(new PolygonAttributes());
    
    QuadArray plane = new QuadArray(4, QuadArray.COORDINATES);  // This makes the plane.
    plane.setCoordinate(0, new Point3f(-limit, -limit, -0.006f));  //You specify your own cornerpoints...
    plane.setCoordinate(1, new Point3f( limit, -limit, -0.006f));
    plane.setCoordinate(2, new Point3f( limit,  limit, -0.006f));
    plane.setCoordinate(3, new Point3f(-limit,  limit, -0.006f));
    objRoot.addChild(new Shape3D(plane, planeAppearance));
       
    return objRoot;
  }

  public final void enableCoordinates(boolean v)
  {
    if (v) {
      //locale.addBranchGraph(coordinates);
      universe.addBranchGraph(coordinates);
    } else {
      universe.getLocale().removeBranchGraph(coordinates);
    }
  }
  
  public final void enableField(boolean v)
  {
    if (v) {
      //locale.addBranchGraph(field);
      universe.addBranchGraph(field);
      //root.addChild(field);
    } else {
      universe.getLocale().removeBranchGraph(field);
      //root.removeChild(field);
    }
  }
  
  public final void enableFieldLab(boolean v)
  {
    if (v) {
      //locale.addBranchGraph(fieldLab);
      universe.addBranchGraph(fieldLab);
      //root.addChild(fieldLab);
    } else {
      universe.getLocale().removeBranchGraph(fieldLab);
      //root.removeChild(fieldLab);
    }
  }

  public void add(BranchGroup bg)
  {
    universe.addBranchGraph(bg);
    //locale.addBranchGraph(bg);
  }
}
