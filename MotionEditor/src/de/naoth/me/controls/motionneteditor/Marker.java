/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.controls.motionneteditor;

import java.awt.Color;
import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class Marker 
{
    private Color color;
    private int radius;
    private int x;
    private int y;
    
    public Marker(int x, int y, Color color, int radius)
    {
        this.color = color;
        this.radius = radius;
        this.x = x;
        this.y = y;
    }
    
    public void draw(Graphics2D g2d)
    {
        g2d.setColor(color); // setze Farbe
        g2d.fillOval(x-radius, y-radius, 2*radius, 2*radius);
    }//end draw
}//end Marker
