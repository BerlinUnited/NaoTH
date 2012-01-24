/*
 * 
 */
package de.naoth.rc.dialogs.drawings;

import de.naoth.rc.math.Vector2D;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.util.Collection;
import java.util.HashMap;
import java.util.Vector;

/**
 *
 * @author Heinrich Mellmann
 */
public class StrokePlot implements Drawable {

  private final HashMap<String, Stroke> plotStrokeMap = new HashMap<String, Stroke>();
  private int maxSize;
  private int colorIndex;

  private double rotation;

  public StrokePlot(int maxSize) {
    this.maxSize = maxSize;
    this.rotation = 0.0;
  }

  public void plot(String id, Vector2D value) {
    plotStrokeMap.get(id).add(value.rotate(rotation));
  }//end update

  // HACK
  public void addStroke(Stroke stroke) {
    if (stroke != null) {
      plotStrokeMap.put(stroke.id, stroke);
    }
  }//end addStroke

  public void addStroke(String id, Color color) {
    if (plotStrokeMap.get(id) != null) {
      return;
    }
    plotStrokeMap.put(id, new Stroke(id, maxSize, color));
  }//end addPlot

  public void setEnabled(String id, boolean value) {
    if (plotStrokeMap.get(id) == null) {
      return;
    }
    plotStrokeMap.get(id).setEnabled(value);
  }//setEnabled

  public void draw(Graphics2D g2d) {
    
    g2d.rotate(-rotation);
    g2d.setStroke(new BasicStroke(25.0f,BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));

    // draw strokes
    drawVector(plotStrokeMap.values(), g2d);
    g2d.rotate(rotation);

    g2d.transform(new AffineTransform(1, 0, 0, -1, 0, 0));
    // draw strings (stroke names)
    int pos = -200;
    for (Stroke stroke : plotStrokeMap.values()) {
      if (!stroke.isEnabled()) {
        continue;
      }
      g2d.setColor(stroke.color);
      g2d.drawString(stroke.id, 50, pos);
      pos += 20;
    }//end for
    
    g2d.transform(new AffineTransform(1, 0, 0, -1, 0, 0));
  }//end draw

  public static void drawVector(Collection<? extends Drawable> drawableVector, Graphics2D g2d) {
    synchronized (drawableVector) {
      for (Drawable elem : drawableVector) {
        synchronized (elem) {
          elem.draw(g2d);
        }//end synchronized
      }//end for
    }//end synchronized
  }//end drawVector

  public Collection<Stroke> getStrokes() {
    return this.plotStrokeMap.values();
  }//end getStrokes
  
  public void clear() {
    this.plotStrokeMap.clear();
  }//end clear

  public void setRotation(double rotation) {
    this.rotation = rotation;
  }

  public static class Stroke implements Drawable {

    private Vector<Vector2D> pointsVector;
    private int maxSize;
    private float r;
    private float g;
    private float b;
    public final String id;
    public final Color color;
    private boolean enabled;

    public Stroke(String id, int maxSize, Color color) {
      this.pointsVector = new Vector<Vector2D>();
      this.maxSize = maxSize;
      this.r = ((float) color.getRed()) / 255.0f;
      this.g = ((float) color.getGreen()) / 255.0f;
      this.b = ((float) color.getBlue()) / 255.0f;
      this.id = id;
      this.color = color;
      this.enabled = false;
    }

    public void add(double x, double y) {
      add(new Vector2D(x, y));
    }

    public void add(Vector2D pose) {
      pointsVector.add(pose);
      if (pointsVector.size() > maxSize) {
        pointsVector.removeElementAt(0);
      }
    }

    public void setEnabled(boolean value) {
      this.enabled = value;
    }//setEnabled

    public boolean isEnabled() {
      return this.enabled;
    }//isEnabled

    public void draw(Graphics2D g2d) {
      if (!enabled) {
        return;
      }

      float n = pointsVector.size();
      Vector2D oldPoint = null;
      float i = 0;
      synchronized (pointsVector) {
        for (Vector2D point : pointsVector) {
          i++;
          g2d.setColor(new Color(r, g, b, i / n));
          if (oldPoint != null) {
            g2d.drawLine((int) oldPoint.x, (int) oldPoint.y, (int) point.x, (int) point.y);
          }

          oldPoint = point;
        }//end for
      }//end synchronized
    }//end draw
  }//end class Stroke
}//end class StrokePlot

