/*
 * 
 */
package de.naoth.rc.drawings;

/**
 *
 * @author Heinrich Mellmann
 */
public class FieldDrawingSPL3x4 extends FieldDrawingSPL2012
{
    @Override
    public String toString() { return "SPL 3x4"; }
    
    public FieldDrawingSPL3x4()
    {
        // FieldInfo 2012 (according the rules)
        this.xLength = 4000;
        this.yLength = 3000;
        this.boundary = 700;

        this.xPenaltyAreaLength = 500;
        this.yPenaltyAreaLength = 1990;

        this.centerCircleRadius = 470;

        this.xCrossOffset = 1300;
        this.crossSize = 100;
        this.fieldLinesWidth = 50;

        // goals
        this.goalWidth = 1600;
        this.goalpostRadius = 50;
        this.goalDepth = 635;
        
        // recalculate the values
        init();
    }
}//end class FieldDrawingSPL2013
