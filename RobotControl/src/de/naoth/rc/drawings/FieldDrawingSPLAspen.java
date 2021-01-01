package de.naoth.rc.drawings;

public class FieldDrawingSPLAspen extends FieldDrawingSPL2012
{
    @Override
    public String toString() { return "SPLAspen"; }
    
    public FieldDrawingSPLAspen()
    {
        // FieldInfo 2012 (according the rules)
        this.xLength = 7500;
        this.yLength = 5000;
        this.boundary = 700;

        this.xPenaltyAreaLength = 600;
        this.yPenaltyAreaLength = 2200;

        this.centerCircleRadius = 750;

        this.xCrossOffset = 1300;
        this.crossSize = 100;
        this.fieldLinesWidth = 50;

        // goals
        this.goalWidth = 1500;
        this.goalpostRadius = 50;
        this.goalDepth = 400 + 2*goalpostRadius;
        
        // recalculate the values
        init();
    }
}//end class FieldDrawingSPLAspen
