/*
 * 
 */
package de.naoth.rc.dialogs.drawings;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class FieldDrawingS3D2011 implements Drawable
{
    private Color fieldColor = new Color(0f, 0.8f, 0f, 0.5f);
    private Color goalColor = new Color(0.9f, 0.9f, 0f, 1f);
    private Color lineColor = Color.white;
    
    @Override
    public String toString() { return "S3D2011"; }
    
    @Override
    public void draw(Graphics2D g2d)
    {
        // draw gree pane
        g2d.setColor(fieldColor);
        g2d.fillRect(-11000, -7500, 22000, 15000);

        // draw lines
        g2d.setColor(lineColor);
        g2d.setStroke(new BasicStroke(50f)); // line width 50mm

        g2d.drawLine(-10500, -7000, 10500, -7000); // bottom side line
        g2d.drawLine(-10500, 7000, 10500, 7000); // top side line
        g2d.drawLine(-10500, -7000, -10500, 7000); // left side line
        g2d.drawLine(10500, -7000, 10500, 7000); // right side line
        g2d.drawLine(0, -7000, 0, 7000); // center line

        g2d.drawLine(-8700, -1950, -8700, 1950); // blue front goal line
        g2d.drawLine(-10500, -1950, -8700, -1950); // blue bottom goal line
        g2d.drawLine(-10500, 1950, -8700, 1950); // blue top goal line

        g2d.drawLine(8700, -1950, 8700, 1950); // yellow front goal line
        g2d.drawLine(10500, -1950, 8700, -1950); // yellow bottom goal line
        g2d.drawLine(10500, 1950, 8700, 1950); // yellow top goal line

        // draw center circle
        // radius is FreeKickDistance*FreeKickDistance
        g2d.drawOval(-1800, -1800, 3600, 3600);


        g2d.setStroke(new BasicStroke(50.0f));
        // draw left goal
        g2d.setColor(goalColor);
        g2d.drawLine(-11000, -1050, -10500, -1050);
        g2d.drawLine(-11000, 1050, -10500, 1050);
        g2d.drawLine(-11000, -1050, -11000, 1050);

        // draw right goal
        g2d.setColor(goalColor);
        g2d.drawLine(11000, -1050, 10500, -1050);
        g2d.drawLine(11000, 1050, 10500, 1050);
        g2d.drawLine(11000, -1050, 11000, 1050);

    }//end draw
}//end FieldDrawingS3D2011
