/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.drawings3d;

import javax.media.j3d.Appearance;
import javax.media.j3d.Background;
import javax.media.j3d.BoundingSphere;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.ColoringAttributes;
import javax.media.j3d.LineArray;
import javax.media.j3d.Locale;
import javax.media.j3d.PolygonAttributes;
import javax.media.j3d.QuadArray;
import javax.media.j3d.Shape3D;
import javax.media.j3d.VirtualUniverse;
import javax.vecmath.Color3f;
import javax.vecmath.Point3d;
import javax.vecmath.Point3f;

/*
 * Singletion class:
 * the java 3D can have one VirtualUniverse only
 */
public class VirtualWorld
{

  private final VirtualUniverse universe;
  private final Locale locale;
 
  private final BranchGroup light;
  private final BranchGroup sky;
  
  // ground
  private final BranchGroup field;
  private final BranchGroup coordinates;
  
  private static VirtualWorld ref = null;

  private VirtualWorld()
  {
    universe = new VirtualUniverse();
    locale = new Locale(universe);
    field = new BranchGroup();
    coordinates = createCoordinateSystem();
    light = new BranchGroup();
    sky = new BranchGroup();
    
    coordinates.setCapability(BranchGroup.ALLOW_DETACH);
    
    // load entities
    field.addChild(new Entity("Field"));
    field.setCapability(BranchGroup.ALLOW_DETACH);
    //locale.addBranchGraph(field);
    enableField(true);
    
    BoundingSphere sphere = new BoundingSphere(new Point3d(0,0,0), 100000);
    Background background = new Background(new Color3f(0.9f,0.9f,0.9f));
    background.setApplicationBounds(sphere);
    sky.addChild(background);
    locale.addBranchGraph(sky);
    
    light.addChild(new Entity("Light"));
    locale.addBranchGraph(light);
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
    LineArray axisXLines=new LineArray((n+1)*4,LineArray.COORDINATES);
    objRoot.addChild(new Shape3D(axisXLines, lineAppearance));
       
    
    for(int i = 0; i <= n; ++i) {
        axisXLines.setCoordinate(4*i+0, new Point3f(i*step-limit,-limit,0.006f));
        axisXLines.setCoordinate(4*i+1, new Point3f(i*step-limit,limit,0.006f));
        
        axisXLines.setCoordinate(4*i+2, new Point3f(-limit, i*step-limit,0.006f));
        axisXLines.setCoordinate(4*i+3, new Point3f(limit, i*step-limit,0.006f));
    }
    
    // ground plane
    Color3f color = new Color3f(0.3f, 0.4f, 0.5f);
    Appearance planeAppearance = new Appearance();
    planeAppearance.setColoringAttributes(new ColoringAttributes(color, ColoringAttributes.SHADE_FLAT));
    planeAppearance.setPolygonAttributes(new PolygonAttributes());
    
    QuadArray plane = new QuadArray (4, QuadArray.COORDINATES);  //This makes the plane.
    plane.setCoordinate(0, new Point3f(-limit, -limit, -0.006f));  //You specify your own cornerpoints...
    plane.setCoordinate(1, new Point3f(limit, -limit, -0.006f));
    plane.setCoordinate(2, new Point3f(limit, limit, -0.006f));
    plane.setCoordinate(3, new Point3f(-limit, limit, -0.006f));
    objRoot.addChild(new Shape3D(plane, planeAppearance));
       
    return objRoot;
  }

  public static VirtualWorld get()
  {
    if (ref == null)
    // it's ok, we can call this constructor
    {
      ref = new VirtualWorld();
    }
    return ref;
  }

  @Override
  public Object clone() throws CloneNotSupportedException
  {
    throw new CloneNotSupportedException();
    // that'll teach 'em
  }

  public final void enableCoordinates(boolean v)
  {
    if (v) {
      locale.addBranchGraph(coordinates);
    } else {
      locale.removeBranchGraph(coordinates);
    }
  }
  
  public final void enableField(boolean v)
  {
    if (v) {
      locale.addBranchGraph(field);
    } else {
      locale.removeBranchGraph(field);
    }
  }

  public void add(BranchGroup bg)
  {
    locale.addBranchGraph(bg);
  }
}
