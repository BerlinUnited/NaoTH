/*
 * 
 */

package de.hu_berlin.informatik.ki.nao.dialogs.drawings;

import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class FillBox extends Box
{
    public FillBox(String[] args)
    {
        super(args);
    }
    
    @Override
    public void draw(Graphics2D g2d)
    {
        g2d.fillRect(x0, y0, width, height);
    }
}//end class FillBox
