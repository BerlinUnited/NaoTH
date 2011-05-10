/*
 * 
 */
package de.hu_berlin.informatik.ki.nao.dialogs.drawings;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;

public class FieldDrawing implements Drawable
{

  private Color leftGoalColor = new Color(0.9f, 0.9f, 0f, 1f);
  private Color rightGoalColor = Color.blue;
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
    // draw gree pane
    g2d.setColor(fieldColor);
    g2d.fillRect(-3700, -2700, 7400, 5400);

    // draw lines
    g2d.setColor(lineColor);
    g2d.setStroke(new BasicStroke(50f)); // line width 50mm

    g2d.drawLine(-3025, -2025, 3025, -2025); // bottom side line
    g2d.drawLine(-3025, 2025, 3025, 2025); // top side line
    g2d.drawLine(-3025, -2025, -3025, 2025); // left side line
    g2d.drawLine(3025, -2025, 3025, 2025); // right side line
    g2d.drawLine(0, -2025, 0, 2025); // center line

    g2d.drawLine(-2400, -1525, -2400, 1525); // blue front goal line
    g2d.drawLine(-3000, -1525, -2400, -1525); // blue bottom goal line
    g2d.drawLine(-3000, 1525, -2400, 1525); // blue top goal line

    g2d.drawLine(2400, -1525, 2400, 1525); // yellow front goal line
    g2d.drawLine(3000, -1525, 2400, -1525); // yellow bottom goal line
    g2d.drawLine(3000, 1525, 2400, 1525); // yellow top goal line

    // draw center circle
    g2d.drawOval(-600, -600, 1200, 1200);

    // draw crosses
    g2d.drawLine(-1125, 0, -1225, 0);
    g2d.drawLine(-1175, -50, -1175, 50);
    g2d.drawLine(1125, 0, 1225, 0);
    g2d.drawLine(1175, -50, 1175, 50);

    g2d.setStroke(new BasicStroke(50.0f));
    // draw left goal
    g2d.setColor(leftGoalColor);
    g2d.drawLine(-3425, -700, -3025, -700);
    g2d.drawLine(-3425, 700, -3025, 700);
    g2d.drawLine(-3425, -700, -3425, 700);
    // goal post
    g2d.fillOval(-3075, 650, 100, 100);
    g2d.fillOval(-3075, -750, 100, 100);

    // draw right goal
    g2d.setColor(rightGoalColor);
    g2d.drawLine(3425, -700, 3025, -700);
    g2d.drawLine(3425, 700, 3025, 700);
    g2d.drawLine(3425, -700, 3425, 700);
    // goal post
    g2d.fillOval(2975, 650, 100, 100);
    g2d.fillOval(2975, -750, 100, 100);

  }//end draw

  private void drawSimsparkField(Graphics2D g2d)
  {
    // draw gree pane
    g2d.setColor(fieldColor);
    g2d.fillRect(-9500, -6500, 19000, 13000);

    // draw lines
    g2d.setColor(lineColor);
    g2d.setStroke(new BasicStroke(50f)); // line width 50mm

    g2d.drawLine(-9000, -6000, 9000, -6000); // bottom side line
    g2d.drawLine(-9000, 6000, 9000, 6000); // top side line
    g2d.drawLine(-9000, -6000, -9000, 6000); // left side line
    g2d.drawLine(9000, -6000, 9000, 6000); // right side line
    g2d.drawLine(0, -6000, 0, 6000); // center line

    g2d.drawLine(-7200, -2100, -7200, 2100); // blue front goal line
    g2d.drawLine(-9000, -2100, -7200, -2100); // blue bottom goal line
    g2d.drawLine(-9000, 2100, -7200, 2100); // blue top goal line

    g2d.drawLine(7200, -2100, 7200, 2100); // yellow front goal line
    g2d.drawLine(9000, -2100, 7200, -2100); // yellow bottom goal line
    g2d.drawLine(9000, 2100, 7200, 2100); // yellow top goal line

    // draw center circle
    // radius is FreeKickDistance*FreeKickDistance
    g2d.drawOval(-1690, -1690, 3380, 3380);


    g2d.setStroke(new BasicStroke(50.0f));
    // draw left goal
    g2d.setColor(leftGoalColor);
    g2d.drawLine(-9600, -1050, -9000, -1050);
    g2d.drawLine(-9600, 1050, -9000, 1050);
    g2d.drawLine(-9600, -1050, -9600, 1050);

    // draw right goal
    g2d.setColor(rightGoalColor);
    g2d.drawLine(9600, -1050, 9000, -1050);
    g2d.drawLine(9600, 1050, 9000, 1050);
    g2d.drawLine(9600, -1050, 9600, 1050);

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