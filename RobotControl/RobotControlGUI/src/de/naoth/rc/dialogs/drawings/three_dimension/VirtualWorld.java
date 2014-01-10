/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.dialogs.drawings.three_dimension;

import javax.media.j3d.Background;
import javax.media.j3d.BoundingSphere;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.Locale;
import javax.media.j3d.VirtualUniverse;
import javax.vecmath.Color3f;
import javax.vecmath.Point3d;

/*
 * Singletion class:
 * the java 3D can have one VirtualUniverse only
 */
public class VirtualWorld
{

  private VirtualUniverse universe;
  private Locale locale;
  private BranchGroup field;
  private BranchGroup light;
  private BranchGroup sky;
  
  private static VirtualWorld ref = null;

  private VirtualWorld()
  {
    universe = new VirtualUniverse();
    locale = new Locale(universe);
    field = new BranchGroup();
    light = new BranchGroup();
    sky = new BranchGroup();
    
    // load entities
    field.addChild(new Entity("Field"));
    field.setCapability(BranchGroup.ALLOW_DETACH);
    locale.addBranchGraph(field);
    
    BoundingSphere sphere = new BoundingSphere(new Point3d(0,0,0), 100000);
    Background background = new Background(new Color3f(0.9f,0.9f,0.9f));
    background.setApplicationBounds(sphere);
    sky.addChild(background);
    locale.addBranchGraph(sky);
    
    light.addChild(new Entity("Light"));
    locale.addBranchGraph(light);
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
  public Object clone()
          throws CloneNotSupportedException
  {
    throw new CloneNotSupportedException();
    // that'll teach 'em
    }

  public void enableField(boolean v)
  {
    if (v)
    {
      locale.addBranchGraph(field);
    } else
    {
      locale.removeBranchGraph(field);
    }
  }

  public void add(BranchGroup bg)
  {
    locale.addBranchGraph(bg);
  }
}
