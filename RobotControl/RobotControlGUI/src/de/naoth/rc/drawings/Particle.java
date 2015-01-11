/*
 * drawing a simple particle with a point and a direction
 */
package de.naoth.rc.drawings;

import java.awt.BasicStroke;
import java.awt.Graphics2D;

/**
 *
 * @author Henrich Mellmann
 */
public class Particle implements Drawable
{
    private int x;
    private int y;
    private double r;

    public Particle(int x, int y, double r) 
    {
        this.x = x;
        this.y = y;
        this.r = r;
    }
    
    public Particle(String[] args)
    {
        this.x = (int)(Double.parseDouble(args[1]));
        this.y = (int)(Double.parseDouble(args[2]));
        this.r = Double.parseDouble(args[3]);
    }
    
    @Override
    public void draw(Graphics2D g2d)
    {
        g2d.translate(x, y);
        g2d.rotate(r);
        
        BasicStroke stroke = (BasicStroke)g2d.getStroke();
        float lineWidth = stroke.getLineWidth();
        g2d.fillOval(-(int)(lineWidth*2), -(int)(lineWidth*2), (int)(lineWidth*4), (int)(lineWidth*4));
        g2d.drawLine(0, 0, (int)(lineWidth*12), 0);
        
        g2d.rotate(-r);
        g2d.translate(-x, -y);
    }//end draw
    
}//end class Particle
