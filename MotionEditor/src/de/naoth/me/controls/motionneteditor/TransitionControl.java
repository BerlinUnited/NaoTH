/*
 */

package de.naoth.me.controls.motionneteditor;

import de.naoth.me.core.Transition;
import java.awt.Color;
import java.awt.Rectangle;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;
import java.awt.geom.Point2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class TransitionControl extends ArrowControl implements MouseMotionListener
{
    
    private final Color mouseOverColor = new Color(128, 128, 193);
    private final Color mouseOutColor = new Color(200, 200, 225);
    private final Color selectedColor = new Color(128, 128, 193);
    
    private boolean selected;

    public boolean isSelected() {
        return selected;
    }
    private boolean focused;
    
    private KeyFrameControl startKeyFrame;
    private KeyFrameControl endKeyFrame;
    private Transition transition;
    
    
    private Point2D from;
    private Point2D to;
    
    private MotionNetEditorPanel canvas;
    
    public TransitionControl(MotionNetEditorPanel canvas, Transition transition, KeyFrameControl startKeyFrame, KeyFrameControl endKeyFrame)
    {
        this.canvas = canvas;
        this.setBounds(canvas.getBounds());
        
        this.startKeyFrame = startKeyFrame;
        this.endKeyFrame = endKeyFrame;

        this.transition = transition;
        
        if(this.startKeyFrame != null) 
            this.startKeyFrame.addMouseMotionListener(this);
        if(this.endKeyFrame != null) 
            this.endKeyFrame.addMouseMotionListener(this);

        addMouseMotionListener(this);
        
        this.setOpaque(false);
        this.setRequestFocusEnabled(true);
        
        setColor(mouseOutColor);
        
        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if(isContain(e.getPoint()))
                    setSelected(true);
            }

            @Override
            public void mousePressed(MouseEvent e) {
                //if(e.isPopupTrigger())
  
                {
                    if(isContain(e.getPoint()))
                        initComponents();
                    else
                    {
                        setInheritsPopupMenu(true);
                        setComponentPopupMenu(null);
                    }
                }
            }
        });
        update();
    }
    
    private void moveToBack()
    {
        canvas.moveToBack(this);
    }
    
    public Transition getTransion()
    {
        return this.transition;
    }
    
    private void initComponents() {

        javax.swing.JPopupMenu jPopupMenu = new javax.swing.JPopupMenu();
        
        javax.swing.JMenuItem jMenuItemDeleteTransition = new javax.swing.JMenuItem();
        jMenuItemDeleteTransition.setText("Remove Transition");
        jPopupMenu.add(jMenuItemDeleteTransition);

        jMenuItemDeleteTransition.addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mouseReleased(java.awt.event.MouseEvent evt) {
                removeThisControl();
            }
        });
        
        setComponentPopupMenu(jPopupMenu);
    }
    
    private void removeThisControl()
    {
        canvas.removeTransitionControl(this);
    }
    
    public void mouseMoved(MouseEvent e)
    {
        setFocused(isContain(e.getPoint()));
        if(selected) setColor(selectedColor);
    }
    
    public void mouseDragged(MouseEvent m)
    {
        update();
    }
    
    public boolean isFocused() {
        return focused;
    }
    
    public void setFocused(boolean focused) {
        
        if(focused)
        {
            this.setColor(mouseOverColor);
            this.canvas.moveToFront(this);
        }else
        {
            this.setColor(mouseOutColor);
            this.canvas.moveToBack(this);
        }
        
        if(focused == this.focused) return;
        
        this.focused = focused;
        repaint();
    }//end setFocused
    
    
    public void setSelected(boolean selected) {
   
        if(selected == this.selected) return;
        this.selected = selected;
        if(this.selected)
        {
            setColor(selectedColor);
            setDrawingStrokeWidth(2f);
            canvas.transitionControlSelected(this);
        }
        else
        {
            setColor(mouseOutColor);
            setDrawingStrokeWidth(1f);
        }
        this.repaint();
    }//end setSelected
 
    

    private void update()
    {
        if(endKeyFrame == null || startKeyFrame == null)
        {
          System.out.println("ups");
          return;
        }

        double radius = (double)(startKeyFrame.getWidth()) / 2.0;

        double c0x = startKeyFrame.getX() + radius;
        double c0y = startKeyFrame.getY() + radius;
        double c1x = endKeyFrame.getX() + radius;
        double c1y = endKeyFrame.getY() + radius;


        double length = Math.sqrt((c1x - c0x)*(c1x - c0x) + (c1y - c0y)*(c1y - c0y));
        double delta = radius/length;

        this.from = new Point2D.Double(c0x+delta*(c1x-c0x), c0y+delta*(c1y-c0y));
        this.to = new Point2D.Double(c1x, c1y);

        this.setP1(new Point2D.Double(from.getX() - this.getX(), from.getY() - this.getY()));
        this.setP2(new Point2D.Double(to.getX() - this.getX(), to.getY() - this.getY()));

        repaint();
    }//end update

}//end class NewTransitionControl
