package de.naoth.rc.drawings;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Stroke;

/**
 *
 * @author Heinrich Mellmann
 */
public class Pen implements Drawable {

    private Color color;
    private Stroke stroke;

    // general pen with color and custom stroke
    public Pen(Color color, Stroke stroke) {
        this.color = color;
        this.stroke = stroke;
    }
    
    // dashed pen
    // dash_period == 0 => solid line
    // dash_period > 0 => dashed line with equal opaque and transparent periods
    public Pen(float width, Color color, float dash_period) {
        this(color, new BasicStroke(width,
                            BasicStroke.CAP_BUTT,
                            BasicStroke.JOIN_MITER,
                            10.0f, // miter limit (default 10.0f)
                            // dashing pattern 
                            dash_period > 0.0f ? new float[]{dash_period} : null, 
                            0.0f // dash start offset
        ));
    }
    
    // simple pen with color and width
    public Pen(float width, Color color) {
        this(width, color, 0.0f);
    }
    
    // construct from string arguments
    // expected structure:
    //    {"Pen", <width>, <color>, [<dash_period>] }
    public Pen(String[] args) {
        this(
            Float.parseFloat(args[2]), 
            Colors.parseColor(args[1]),
            args.length > 3 ? Float.parseFloat(args[3]) : 0.0f
        );
    }


    @Override
    public void draw(Graphics2D g2d) {
        g2d.setColor(color);
        g2d.setStroke(stroke);
    }
}//end class Pen

