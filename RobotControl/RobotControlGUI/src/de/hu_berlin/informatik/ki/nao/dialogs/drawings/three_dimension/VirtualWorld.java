/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.hu_berlin.informatik.ki.nao.dialogs.drawings.three_dimension;

import javax.media.j3d.BranchGroup;
import javax.media.j3d.Locale;
import javax.media.j3d.VirtualUniverse;

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
  private static VirtualWorld ref = null;

  private VirtualWorld()
  {
    universe = new VirtualUniverse();
    locale = new Locale(universe);
    field = new BranchGroup();
    light = new BranchGroup();
    
    // load entities
    field.addChild(new Entity("env/Field"));
    field.setCapability(BranchGroup.ALLOW_DETACH);
    locale.addBranchGraph(field);
    light.addChild(new Entity("env/Light"));
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
