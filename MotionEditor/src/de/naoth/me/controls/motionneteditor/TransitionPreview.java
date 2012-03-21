/*
 */

package de.naoth.me.controls.motionneteditor;

import java.awt.Color;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;
import java.awt.geom.Point2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class TransitionPreview extends ArrowControl implements MouseMotionListener
{
    private KeyFrameControl startKeyFrame;
    private Point2D from;
    private Point2D to;
    private MotionNetEditorPanel canvas;
       
    public TransitionPreview(MotionNetEditorPanel parent, KeyFrameControl startKeyFrame)
    {
        this.canvas = parent;
        this.setBounds(parent.getBounds());
        this.startKeyFrame = startKeyFrame;
        this.setColor(Color.LIGHT_GRAY);
        
        this.setOpaque(false);
        this.setRequestFocusEnabled(true);
        
        addMouseMotionListener(this);
        this.canvas.moveToBack(this);
        
        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                canvas.cancelCreateNewTransition();
            }
        });
    }
    
    public void mouseMoved(MouseEvent e)
    {
        if(e.getComponent() == this)
            update(e.getX() + this.getX(), e.getY() + this.getY());
        else
            update(e.getX(), e.getY());
    }
    
    public void mouseDragged(MouseEvent e)
    {
         if(e.getComponent() == this)
            update(e.getX() + this.getX(), e.getY() + this.getY());
        else
            update(e.getX(), e.getY());
    }
    
    private void update(int x, int y)
    {
        if(startKeyFrame == null) return;
        
        double radius = (double)(startKeyFrame.getWidth()) / 2.0;
        
        double c0x = startKeyFrame.getX() + radius;
        double c0y = startKeyFrame.getY() + radius;
        double c1x = x;
        double c1y = y;
        
        
        double length = Math.sqrt((c1x - c0x)*(c1x - c0x) + (c1y - c0y)*(c1y - c0y));
        double delta = radius/length;
        
        this.from = new Point2D.Double(c0x+delta*(c1x-c0x), c0y+delta*(c1y-c0y));
        this.to = new Point2D.Double(c1x, c1y);
        
        this.setP1(new Point2D.Double(from.getX() - this.getX(), from.getY() - this.getY()));
        this.setP2(new Point2D.Double(to.getX() - this.getX(), to.getY() - this.getY()));
        
        repaint();
        
    }//end updatePreview
}//end class TransitionPreview
