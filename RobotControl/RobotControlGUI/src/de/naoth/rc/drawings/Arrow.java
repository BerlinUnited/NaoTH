/*
 * 
 */

package de.naoth.rc.drawings;

import java.awt.BasicStroke;
import java.awt.Graphics2D;
import java.awt.Polygon;

/**
 *
 * @author Heinrich Mellmann
 */
public class Arrow implements Drawable
{
    private final int x0;
    private final int y0;
    private final int x1;
    private final int y1;

    public Arrow(
            int x0,
            int y0,
            int x1,
            int y1)
    {
        this.x0 = x0;
        this.y0 = y0;
        this.x1 = x1;
        this.y1 = y1;
    }
    
    public Arrow(String[] args)
    {
        this.x0 = (int)(Double.parseDouble(args[1]));
        this.y0 = (int)(Double.parseDouble(args[2]));
        this.x1 = (int)(Double.parseDouble(args[3]));
        this.y1 = (int)(Double.parseDouble(args[4]));
    }
    
    @Override
    public void draw(Graphics2D g2d)
    {
        g2d.drawLine(x0, y0, x1, y1);
        
        // draw arrow head
        BasicStroke oldStroke = (BasicStroke)g2d.getStroke();
        float lineWidth = oldStroke.getLineWidth();
        
        g2d.setStroke(new BasicStroke(1f));
        
        
        double lineAngle=Math.atan2(x1-x0, y1-y0);
        
        g2d.translate(x1,y1);
        g2d.rotate(-lineAngle);
        drawArrowHead(g2d, (int)(lineWidth*2+0.5), (int)(lineWidth*4+0.5));
        g2d.rotate(lineAngle);
        g2d.translate(-x1,-y1);
        
        g2d.setStroke(oldStroke);
    }
    
    private void drawArrowHead(Graphics2D g2d, int width, int height) {

        double arrowAngle = 2.1;
        int xOffset = (int)(width * Math.sin(arrowAngle)+0.5);
        int yOffset = (int)(width * Math.cos(arrowAngle)+0.5);
        
        Polygon arrowHead = new Polygon(
                new int[]{0, -xOffset, 0, xOffset, 0},
                new int[]{-height/2, yOffset-height/2, height/2, yOffset-height/2, -height/2},
                5 );
       
       g2d.fillPolygon(arrowHead);
    } 
    
   private void drawArrowHeadSimple(Graphics2D g2d, int width, int height) {
       Polygon arrowHead = new Polygon(
                new int[]{0       ,   width/2,  -width/2, 0},
                new int[]{height  , 0, 0, height},
                4 );
       
       g2d.fillPolygon(arrowHead);
   }

}//end class Arrow
