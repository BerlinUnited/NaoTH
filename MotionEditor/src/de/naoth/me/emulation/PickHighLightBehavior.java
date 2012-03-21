/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;

import javax.media.j3d.*;
import com.sun.j3d.utils.picking.PickTool;
import com.sun.j3d.utils.picking.PickResult;
import com.sun.j3d.utils.picking.behaviors.PickMouseBehavior;
//import java.util.*;
//import java.awt.*;
//import java.awt.Event;
//import java.awt.AWTEvent;
//import java.awt.event.MouseEvent;
import javax.vecmath.*;


/**
 *
 * @author claas
 */
public class PickHighLightBehavior extends PickMouseBehavior
{
  Appearance savedAppearance = null;
  Shape3D oldShape = null;
  Appearance highlightAppearance;

  public PickHighLightBehavior(Canvas3D canvas, BranchGroup root, Bounds bounds)
  {
      super(canvas, root, bounds);
      this.setSchedulingBounds(bounds);
//      root.addChild(this);
//      Color3f white = new Color3f(1.0f, 1.0f, 1.0f);
      Color3f yellow = new Color3f(0.8f, 0.0f, 0.8f);
//      Color3f black = new Color3f(0.0f, 0.0f, 0.0f);
//      Color3f highlightColor = new Color3f(0.0f, 1.0f, 0.0f);
//      Material highlightMaterial = new Material(highlightColor, black, highlightColor, white,	80.0f);

//      Color3f lightGray = new Color3f(0.8f, 0.8f, 0.8f);
      ColoringAttributes ca = new ColoringAttributes(yellow, ColoringAttributes.NICEST);
//      app.setColoringAttributes(ca);
      highlightAppearance = new Appearance();
//      highlightAppearance.setMaterial(highlightMaterial);
      highlightAppearance.setColoringAttributes(ca);

      pickCanvas.setMode(PickTool.BOUNDS);
  }

  public void updateScene(int xpos, int ypos)
  {
    PickResult pickResult = null;
    Shape3D shape = null;
    Transform3D t3d = null;
    TransformGroup tg = null;

//    pickCanvas.setShapeLocation(xpos, ypos);

    pickResult = pickCanvas.pickClosest();
    if (pickResult != null)
    {
//        shape = (Shape3D) pickResult.getNode(PickResult.SHAPE3D);
        tg = (TransformGroup) pickResult.getNode(PickResult.TRANSFORM_GROUP);
        tg.getTransform(t3d);
        t3d.rotX(0.4);
        tg.setTransform(t3d);
    }

//    if (oldShape != null)
//    {
//        oldShape.setAppearance(savedAppearance);
//    }
//    if (shape != null)
//    {
//        savedAppearance = shape.getAppearance();
//        oldShape = shape;
//        shape.setAppearance(highlightAppearance);
//    }
  }
}

