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
public class FieldDrawingSPL2013BlackWhite implements Drawable
{
    
    // ------------------------------
    // FieldInfo 2012 (according the rules)
    int xLength = 9000;
    int yLength = 6000;
    int boundary = 700;
    
    int xPenaltyAreaLength = 600;
    int yPenaltyAreaLength = 2200;
    
    int centerCircleRadius = 750;
    
    int xCrossOffset = 1800;
    int crossSize = 100;
    int fieldLinesWidth = 50;
    
    // goals
    int goalWidth = 1500;
    int goalpostRadius = 50;
    int goalDepth = 400 + 2*goalpostRadius;
    
    // ------------------------------
    
    
    // size of the carpet
    int xFieldLength;
    int yFieldLength;

    int xLengthHalf;
    int yLengthHalf;

    int xCrossPos;
    int yGoalPostPos;
    int yPosLeftPenaltyArea;
    
    // some usefull precalculations
    protected final void init()
    {
        // size of the carpet
        this.xFieldLength = xLength + 2*boundary;
        this.yFieldLength = yLength + 2*boundary;

        this.xLengthHalf = xLength/2;
        this.yLengthHalf = yLength/2;

        this.xCrossPos = xLengthHalf - xCrossOffset;
        this.yGoalPostPos = goalWidth/2;
        this.yPosLeftPenaltyArea = yPenaltyAreaLength/2;
    }
    
    
    private final Color fieldColor = new Color(0.0f, 0.0f, 0.0f, 0.0f);
    private final Color goalColor = new Color(0.9f, 0.9f, 0f, 1f);
    private final Color goalNetColorOwn = new Color(0.0f, 0.0f, 0.0f, 0.0f);
    private final Color goalNetColorOpp = new Color(0.0f, 0.0f, 0.0f, 0.0f);
    private final Color lineColor = Color.black;
    
    
    public FieldDrawingSPL2013BlackWhite()
    {
        init();
    }
    
    @Override
    public String toString() { return "SPL2013-BlackWhite"; }
    
    @Override
    public void draw(Graphics2D g2d)
    {
        // draw gree pane
        g2d.setColor(fieldColor);
        g2d.fillRect(-xFieldLength/2, -yFieldLength/2, xFieldLength, yFieldLength);

        // draw lines
        g2d.setColor(lineColor);
        g2d.setStroke(new BasicStroke(fieldLinesWidth, BasicStroke.CAP_BUTT, BasicStroke.JOIN_MITER)); // line type

        // outer boundary
        g2d.drawRect(-xLengthHalf, -yLengthHalf, xLength, yLength);
        g2d.drawLine(           0, -yLengthHalf,            0,  yLengthHalf); // center line

        // left penalty area
        g2d.drawRect(-xLengthHalf, -yPosLeftPenaltyArea, xPenaltyAreaLength, yPenaltyAreaLength);

        // right penalty area
        g2d.drawRect(xLengthHalf-xPenaltyAreaLength, -yPosLeftPenaltyArea, xPenaltyAreaLength, yPenaltyAreaLength);

        // draw center circle
        g2d.drawOval(-centerCircleRadius, -centerCircleRadius, centerCircleRadius*2, centerCircleRadius*2);
        g2d.drawLine(-crossSize/2, 0, crossSize/2, 0);

        // draw crosses
        g2d.drawLine(-xCrossPos-crossSize/2, 0, -xCrossPos+crossSize/2, 0);
        g2d.drawLine(-xCrossPos, -crossSize/2 , -xCrossPos, crossSize/2);

        g2d.drawLine(xCrossPos-crossSize/2, 0, xCrossPos+crossSize/2, 0);
        g2d.drawLine(xCrossPos, -crossSize/2, xCrossPos, crossSize/2);


        // goal box/net
        g2d.setStroke(new BasicStroke(1.0f));
        g2d.setColor(lineColor);
        g2d.fillRect(-xLengthHalf-goalpostRadius/2 - goalDepth, -yGoalPostPos, goalDepth, goalWidth);
        g2d.setColor(Color.white);
        g2d.fillRect(-xLengthHalf-goalpostRadius/2 - goalDepth+20, -yGoalPostPos+20, goalDepth-40, goalWidth-40);
        // goal --- latte :)
        g2d.setColor(goalColor);
        g2d.fillRect(-xLengthHalf-goalpostRadius-goalpostRadius/2, -yGoalPostPos, goalpostRadius*2, goalWidth);

        // goal post
        g2d.fillOval(-xLengthHalf-goalpostRadius-goalpostRadius/2, yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);
        g2d.fillOval(-xLengthHalf-goalpostRadius-goalpostRadius/2, -yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);

        //
        g2d.setStroke(new BasicStroke(1.0f));
        g2d.setColor(Color.black);
        g2d.drawOval(-xLengthHalf-goalpostRadius-goalpostRadius/2, yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);
        g2d.drawOval(-xLengthHalf-goalpostRadius-goalpostRadius/2, -yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);


        
        // goal box/net
        g2d.setStroke(new BasicStroke(1.0f));
        g2d.setColor(lineColor);
        g2d.fillRect(xLengthHalf+goalpostRadius/2, -yGoalPostPos, goalDepth, goalWidth);
        g2d.setColor(Color.white);
        g2d.fillRect(xLengthHalf+goalpostRadius/2+20, -yGoalPostPos+20, goalDepth-40, goalWidth-40);

        // goal --- latte :)
        g2d.setColor(goalColor);
        g2d.fillRect(xLengthHalf-goalpostRadius+goalpostRadius/2, -yGoalPostPos, goalpostRadius*2, goalWidth);

        // goal post
        g2d.fillOval(xLengthHalf-goalpostRadius+goalpostRadius/2, yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);
        g2d.fillOval(xLengthHalf-goalpostRadius+goalpostRadius/2, -yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);

        //
        g2d.setStroke(new BasicStroke(1.0f));
        g2d.setColor(Color.black);
        g2d.drawOval(xLengthHalf-goalpostRadius+goalpostRadius/2, yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);
        g2d.drawOval(xLengthHalf-goalpostRadius+goalpostRadius/2, -yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);

    }//end draw
    
}//end class FieldDrawingSPL2012
