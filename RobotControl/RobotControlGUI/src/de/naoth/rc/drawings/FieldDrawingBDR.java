/*
 * 
 */
package de.naoth.rc.drawings;

/**
 *
 * @author Heinrich Mellmann
 */
public class FieldDrawingBDR extends FieldDrawingSPL2012
{
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
        
        // recalculate the values
        init();
    }
}//end class FieldDrawingBDR
