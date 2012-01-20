/*
 * 
 */
package de.naoth.rc.dialogs.drawings;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;

public class FieldDrawing implements Drawable
{

  private Color leftGoalColor = new Color(0.9f, 0.9f, 0f, 1f);
  private Color rightGoalColor = leftGoalColor;//Color.blue;
  private Color fieldColor = new Color(0f, 0.8f, 0f, 0.5f);
  private Color lineColor = Color.white;

  private boolean simsparkFieldSize = false;

  public void draw(Graphics2D g2d)
  {
    if(simsparkFieldSize)
    {
      drawSimsparkField(g2d);
    }
    else
    {
      drawSPLField(g2d);
    }
  }

  private void drawSPLField(Graphics2D g2d)
  {
    // FieldInfo
    int xFieldLength = 7400;
    int yFieldLength = 5400;
    int xLength = 6000;
    int yLength = 4000;

    int xPosOpponentPenaltyArea = 2400;
    int yPosLeftPenaltyArea = 1100;
    int xCrossOffset = 1800;

    int centerCircleRadius = 600;
    int fieldLinesWidth = 50;
    int goalWidth = 1400;
    int goalHeight = 800;
    int goalpostRadius = 50;
    int crossSize = 100;
      
    // some precalculations
    int halfLine = fieldLinesWidth/2;
    int xLengthHalf = xLength/2;
    int yLengthHalf = yLength/2;
    int xCrossPos = xLengthHalf - xCrossOffset;
    int yGoalPostPos = goalWidth/2;
    int goalDepth = 400;
    
    // draw gree pane
    g2d.setColor(fieldColor);
    g2d.fillRect(-xFieldLength/2, -yFieldLength/2, xFieldLength, yFieldLength);

    // draw lines
    g2d.setColor(lineColor);
    g2d.setStroke(new BasicStroke(fieldLinesWidth)); // line width

    // outer boundary
    g2d.drawLine(-xLengthHalf, -yLengthHalf,  xLengthHalf, -yLengthHalf); // bottom side line
    g2d.drawLine(-xLengthHalf,  yLengthHalf,  xLengthHalf,  yLengthHalf); // top side line
    g2d.drawLine(-xLengthHalf, -yLengthHalf, -xLengthHalf,  yLengthHalf); // left side line
    g2d.drawLine( xLengthHalf, -yLengthHalf,  xLengthHalf,  yLengthHalf); // right side line
    g2d.drawLine(           0, -yLengthHalf,            0,  yLengthHalf); // center line

    // blue penalty area
    g2d.drawLine(-xPosOpponentPenaltyArea, -yPosLeftPenaltyArea, -xPosOpponentPenaltyArea, yPosLeftPenaltyArea); // blue front goal line
    g2d.drawLine(-xLengthHalf, -yPosLeftPenaltyArea, -xPosOpponentPenaltyArea, -yPosLeftPenaltyArea); // blue bottom goal line
    g2d.drawLine(-xLengthHalf, yPosLeftPenaltyArea, -xPosOpponentPenaltyArea, yPosLeftPenaltyArea); // blue top goal line

    // yellow penalty area
    g2d.drawLine(xPosOpponentPenaltyArea, -yPosLeftPenaltyArea, xPosOpponentPenaltyArea, yPosLeftPenaltyArea); // yellow front goal line
    g2d.drawLine(xLengthHalf, -yPosLeftPenaltyArea, xPosOpponentPenaltyArea, -yPosLeftPenaltyArea); // yellow bottom goal line
    g2d.drawLine(xLengthHalf, yPosLeftPenaltyArea, xPosOpponentPenaltyArea, yPosLeftPenaltyArea); // yellow top goal line

    // draw center circle
    g2d.drawOval(-centerCircleRadius, -centerCircleRadius, centerCircleRadius*2, centerCircleRadius*2);

    // draw crosses
    g2d.drawLine(-xCrossPos-crossSize/2, 0, -xCrossPos+crossSize/2, 0);
    g2d.drawLine(-xCrossPos, -crossSize/2, -xCrossPos, crossSize/2);
    
    g2d.drawLine(xCrossPos-crossSize/2, 0, xCrossPos+crossSize/2, 0);
    g2d.drawLine(xCrossPos, -crossSize/2, xCrossPos, crossSize/2);

    g2d.setStroke(new BasicStroke(50.0f));
    // draw left goal
    g2d.setColor(leftGoalColor);
    g2d.drawLine(-xLengthHalf-goalDepth, -yGoalPostPos, -xLengthHalf, -yGoalPostPos);
    g2d.drawLine(-xLengthHalf-goalDepth,  yGoalPostPos, -xLengthHalf,  yGoalPostPos);
    g2d.drawLine(-xLengthHalf-goalDepth, -yGoalPostPos, -xLengthHalf-goalDepth,  yGoalPostPos);
    // goal post
    g2d.fillOval(-xLengthHalf-goalpostRadius, yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);
    g2d.fillOval(-xLengthHalf-goalpostRadius, -yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);

    // draw right goal
    g2d.setColor(rightGoalColor);
    g2d.drawLine(xLengthHalf+goalDepth, -yGoalPostPos, xLengthHalf, -yGoalPostPos);
    g2d.drawLine(xLengthHalf+goalDepth,  yGoalPostPos, xLengthHalf,  yGoalPostPos);
    g2d.drawLine(xLengthHalf+goalDepth, -yGoalPostPos, xLengthHalf+goalDepth,  yGoalPostPos);
    // goal post
    g2d.fillOval(xLengthHalf-goalpostRadius, yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);
    g2d.fillOval(xLengthHalf-goalpostRadius, -yGoalPostPos-goalpostRadius, goalpostRadius*2, goalpostRadius*2);

  }//end draw

  private void drawSimsparkField(Graphics2D g2d)
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
    g2d.setColor(leftGoalColor);
    g2d.drawLine(-11000, -1050, -10500, -1050);
    g2d.drawLine(-11000, 1050, -10500, 1050);
    g2d.drawLine(-11000, -1050, -11000, 1050);

    // draw right goal
    g2d.setColor(rightGoalColor);
    g2d.drawLine(11000, -1050, 10500, -1050);
    g2d.drawLine(11000, 1050, 10500, 1050);
    g2d.drawLine(11000, -1050, 11000, 1050);

  }//end draw

  public void switchGoals()
  {
    Color tmpColor = leftGoalColor;
    leftGoalColor = rightGoalColor;
    rightGoalColor = tmpColor;
  }//end switchGoals

  public boolean isSimsparkFieldSize()
  {
    return simsparkFieldSize;
  }

  public void setSimsparkFieldSize(boolean simsparkFieldSize)
  {
    this.simsparkFieldSize = simsparkFieldSize;
  }



}//end class FieldDrawing