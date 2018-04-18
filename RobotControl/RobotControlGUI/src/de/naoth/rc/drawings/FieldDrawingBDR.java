/*
 * 
 */
package de.naoth.rc.drawings;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;

/**
 *
 * @author Heinrich Mellmann
 */
public class FieldDrawingBDR extends FieldDrawingSPL2012
{
    int benchWidth = 800;
    int benchLength = 150;
    
    @Override
    public String toString() { return "BDR"; }
    
    public FieldDrawingBDR()
    {
        this.xLength = 1806;
        this.yLength = 1112;
        this.boundary = 50;

        this.xPenaltyAreaLength = 216;
        this.yPenaltyAreaLength = 478;

        this.centerCircleRadius = 170;

        this.xCrossOffset = 300;
        this.crossSize = 10;
        this.fieldLinesWidth = 10;

        // goals
        this.goalWidth = 378;
        this.goalpostRadius = 10;
        this.goalDepth = 50;
        
        this.fieldColor = new Color(0,90,120);
        this.fieldColor = new Color(0,80,110);
        
        // recalculate the values
        init();
    }

    @Override
    public void draw(Graphics2D g2d) {
        super.draw(g2d);
        
        // goal box/net
        g2d.setStroke(new BasicStroke(1.0f));
        g2d.setColor(Color.ORANGE);
        g2d.fillRect(-benchWidth/2, yLength / 2 + boundary * 2, benchWidth, benchLength);
    }
    
    
}//end class FieldDrawingBDR
