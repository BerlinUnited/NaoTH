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
    
    int boxWidth;
    int boxLength;
    int wallThickness;
    
    @Override
    public String toString() { return "BDR"; }
    
    public FieldDrawingBDR()
    {
        this.xLength = 1806;
        this.yLength = 1112;
        this.boundary = 90;

        this.xPenaltyAreaLength = 210;
        this.yPenaltyAreaLength = 500;

        this.centerCircleRadius = 175;

        this.xCrossOffset = 300;
        this.crossSize = 10;
        this.fieldLinesWidth = 15;

        // goals
        this.goalWidth = 378;
        this.goalpostRadius = 10;
        this.goalDepth = 50;
        
        this.fieldColor = new Color(0,90,120);
        this.fieldColor = new Color(0,80,110);
        
        // a litte lighter
        this.fieldColor = new Color(50+0,50+80,50+110);
        
        this.wallThickness = 20;
        this.boxLength = this.xLength + 2*this.boundary + this.wallThickness;
        this.boxWidth = this.yLength + 2*this.boundary + 410 + this.wallThickness;
        
        // recalculate the values
        init();
    }

    @Override
    public void draw(Graphics2D g2d) {
        
        // stand
        g2d.setStroke(new BasicStroke(this.wallThickness));
        g2d.setColor(new Color(230,230,230));
        g2d.fillRect(-this.boxLength/2, -this.yFieldLength/2-this.wallThickness/2, this.boxLength, this.boxWidth);
        g2d.setColor(Color.darkGray);
        g2d.drawRect(-this.boxLength/2, -this.yFieldLength/2-this.wallThickness/2, this.boxLength, this.boxWidth);
        g2d.setStroke(new BasicStroke(this.wallThickness/2));
        // make it look like double border
        //g2d.setColor(Color.lightGray);
        //g2d.drawRect(-this.boxLength/2, -this.yFieldLength/2-this.wallThickness/2, this.boxLength, this.boxWidth);
        
        super.draw(g2d);
        
        // goal box/net
        g2d.setStroke(new BasicStroke(1.0f));
        g2d.setColor(Color.ORANGE);
        g2d.fillRect(-benchWidth/2, yLength / 2 + boundary * 2, benchWidth, benchLength);
        g2d.setColor(Color.black);
        g2d.drawRect(-benchWidth/2, yLength / 2 + boundary * 2, benchWidth, benchLength);
    }
    
    
}//end class FieldDrawingBDR
