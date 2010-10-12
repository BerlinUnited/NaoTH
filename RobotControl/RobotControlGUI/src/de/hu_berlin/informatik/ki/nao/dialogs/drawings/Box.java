/*
 * 
 */

package de.hu_berlin.informatik.ki.nao.dialogs.drawings;

import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class Box implements Drawable
{
    protected int x0;
    protected int y0;
    protected int width;
    protected int height;

    public Box(String[] args)
    {
        this.x0 = (int)(Double.parseDouble(args[1]));
        this.y0 = (int)(Double.parseDouble(args[2]));
        int x1 = (int)(Double.parseDouble(args[3]));
        int y1 = (int)(Double.parseDouble(args[4]));
        
        
        this.width = Math.abs(x1-this.x0);
        this.height = Math.abs(y1-this.y0);
        this.x0 = Math.min(this.x0, x1);
        this.y0 = Math.min(this.y0, y1);
    }

    public void draw(Graphics2D g2d)
    {
        g2d.drawRect(x0, y0, width, height);
    }
}//end Line
