/*
 * DynamicCanvasPanel.java
 *
 * Created on 8. August 2008, 18:43
 */
package de.naoth.rc.dialogs.panels;

import de.naoth.rc.dialogs.drawings.Drawable;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.RenderingHints;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.geom.AffineTransform;
import java.util.ArrayList;

/**
 *
 * @author  Heinrich Mellmann
 */
public class DynamicCanvasPanel extends javax.swing.JPanel implements MouseMotionListener, MouseListener, MouseWheelListener
{

  private double offsetX;
  private double offsetY;
  private double scale;
  private double rotation;
  private boolean mirrorXAxis = true;
  private double dragOffsetX;
  private double dragOffsetY;
  private ArrayList<Drawable> drawingList;
  private boolean antializing;

  public DynamicCanvasPanel()
  {
    this(true);
  }

  /** Creates new form DynamicCanvasPanel */
  public DynamicCanvasPanel(boolean interactive)
  {
    initComponents();

    if (interactive)
    {
      this.addMouseMotionListener(this);
      this.addMouseListener(this);
      this.addMouseWheelListener(this);
    }
    this.setOpaque(true);

    drawingList = new ArrayList<Drawable>();

    this.offsetX = 0.0;
    this.offsetY = 0.0;
    this.scale = 1.0;

    this.antializing = false;
    this.rotation = 0.0;
  }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentResized(java.awt.event.ComponentEvent evt) {
                formComponentResized(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 400, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 300, Short.MAX_VALUE)
        );
    }// </editor-fold>//GEN-END:initComponents

  int oldWidth = 0;
  int oldHeight = 0;
  private void formComponentResized(java.awt.event.ComponentEvent evt)//GEN-FIRST:event_formComponentResized
  {//GEN-HEADEREND:event_formComponentResized
      if(oldWidth != 0 && oldHeight != 0)
      {
          double sw = ((double)this.getWidth()) / ((double)oldWidth);
          double sh = ((double)this.getHeight()) / ((double)oldHeight);
          
          double s = (Math.abs(sh-1.0) < Math.abs(sw-1.0))? sw: sh;
          
          this.scale *= s;
          this.offsetX = (int)( ((double)this.offsetX) * s +0.5);
          this.offsetY = (int)( ((double)this.offsetY) * s +0.5);
      }
      
      oldWidth = this.getWidth();
      oldHeight = this.getHeight();
      
      
      
      this.repaint();
  }//GEN-LAST:event_formComponentResized

    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables
  public void addDrawing(Drawable drawing)
  {
    this.drawingList.add(drawing);
  }//end addDrawing

  public void removeDrawing(Drawable drawing)
  {
    this.drawingList.remove(drawing);
  }//end removeDrawing

  @Override
  public String getToolTipText(MouseEvent e)
  {
      Point.Double p = canvasCoordinatesToInternal(new Point.Double(e.getX(), e.getY()));
      String s = String.format("%.0f;%.0f", p.x, p.y);
      return s;
  }

  @Override
  public Point getToolTipLocation(MouseEvent e)
  {
    Point p = e.getPoint();
    p.y += 15;
    return p;
  }
  
  public void setShowCoordinates(boolean v) {
      
  }
  
  @Override
  public synchronized void paintComponent(Graphics g)
  {
    super.paintComponent(g);
    Graphics2D g2d = (Graphics2D) g;
    if (this.antializing)
    {
      g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
      g2d.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
      g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);
      g2d.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
    }

    // transform the coordinate system to the mathematical one ;)

    g2d.translate(offsetX, offsetY);
    g2d.rotate(rotation);
    if(mirrorXAxis)
    {
      g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    }
    g2d.scale(scale, scale);

    synchronized (drawingList)
    {
      for (Drawable object : drawingList)
      {
        object.draw(g2d);
      }
    }//end synchronized

    // transform the drawing-pane back (nessesary to draw the other components corect)
    g2d.scale(1.0/scale, 1.0/scale);
    if(mirrorXAxis)
    {
      g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    }
    g2d.rotate(-rotation);
    g2d.translate(-offsetX, -offsetY);
  }//end paintComponent

  // <editor-fold defaultstate="collapsed" desc="Handling Mouse Events">
  @Override
  public void mouseWheelMoved(MouseWheelEvent e)
  {
    double newScale = scale - scale * e.getWheelRotation() * 0.1;
    if (newScale < 15)
    {
      double newOffsetX = (offsetX - e.getX()) * newScale / scale + e.getX();
      double newOffsetY = (offsetY - e.getY()) * newScale / scale + e.getY();

      dragOffsetX += newOffsetX - offsetX;
      dragOffsetY += newOffsetY - offsetY;

      offsetX = newOffsetX;
      offsetY = newOffsetY;
      scale = newScale;
      repaint();
    }//end if
  }//end mouseWheelMoved

  @Override
  public void mouseDragged(MouseEvent e)
  {
    this.offsetX = this.dragOffsetX + e.getX();
    this.offsetY = this.dragOffsetY + e.getY();
    this.repaint();
  }

  @Override
  public void mousePressed(MouseEvent e)
  {
    this.dragOffsetX = offsetX - e.getX();
    this.dragOffsetY = offsetY - e.getY();
  }//end mousePressed

  @Override
  public void mouseReleased(MouseEvent e)
  {
  }

  @Override
  public void mouseEntered(MouseEvent e)
  {
  }

  @Override
  public void mouseExited(MouseEvent e)
  {
  }

  @Override
  public void mouseMoved(MouseEvent e)
  {
  }

  @Override
  public void mouseClicked(MouseEvent e)
  {
  }

  // </editor-fold>
  // <editor-fold defaultstate="collapsed" desc="Getter and Setter">
  
  public Point.Double canvasCoordinatesToInternal(Point.Double p)
  {
    Point.Double result = new Point.Double();
    result.x = (p.x - offsetX)/scale;
    result.y = (p.y - offsetY)/scale;
    return result;
  }

  public void setOffsetX(double offsetX)
  {
    this.offsetX = offsetX;
  }

  public void setOffsetY(double offsetY)
  {
    this.offsetY = offsetY;
  }

  public void setScale(double scale)
  {
    this.scale = scale;
  }

  public synchronized ArrayList<Drawable> getDrawingList()
  {
    return drawingList;
  }

  public double getOffsetX()
  {
    return offsetX;
  }

  public double getOffsetY()
  {
    return offsetY;
  }

  public double getScale()
  {
    return scale;
  }

  public double getRotation()
  {
    return rotation;
  }

  public void setRotation(double rotation)
  {
    this.rotation = rotation;
  }

  public boolean isAntializing()
  {
    return antializing;
  }

  public void setAntializing(boolean antializing)
  {
    this.antializing = antializing;
  }

  public boolean isMirrorXAxis()
  {
    return mirrorXAxis;
  }

  public void setMirrorXAxis(boolean mirrorXAxis)
  {
    this.mirrorXAxis = mirrorXAxis;
  }
  // </editor-fold>
}//end DynamicCanvasPanel

