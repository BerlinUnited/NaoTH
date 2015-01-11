/*
 * 
 */
package de.naoth.rc.drawings;

/**
 *
 * @author Heinrich Mellmann
 */
public class FieldDrawingSPL2013 extends FieldDrawingSPL2012
{
    @Override
    public String toString() { return "SPL2013"; }
    
    public FieldDrawingSPL2013()
    {
        // FieldInfo 2012 (according the rules)
        this.xLength = 9000;
        this.yLength = 6000;
        this.boundary = 700;

        this.xPenaltyAreaLength = 600;
        this.yPenaltyAreaLength = 2200;

        this.centerCircleRadius = 750;

        this.xCrossOffset = 1800;
        this.crossSize = 100;
        this.fieldLinesWidth = 50;

        // goals
        this.goalWidth = 1500;
        this.goalpostRadius = 50;
        this.goalDepth = 400 + 2*goalpostRadius;
        
        // recalculate the values
        init();
    }
}//end class FieldDrawingSPL2013
