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

    public Pen(float width, Color color) {
        this(color, new BasicStroke(width));
    }

    public Pen(String[] args) {
        this((Float.parseFloat(args[2])), Colors.parseColor(args[1]));
    }

    public Pen(Color color, Stroke stroke) {
        this.color = color;
        this.stroke = stroke;
    }

    @Override
    public void draw(Graphics2D g2d) {
        g2d.setColor(color);
        g2d.setStroke(stroke);
    }
}//end class Pen

