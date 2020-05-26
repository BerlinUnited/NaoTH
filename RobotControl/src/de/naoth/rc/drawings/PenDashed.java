package de.naoth.rc.drawings;

import java.awt.BasicStroke;
import java.awt.Color;

/**
 *
 * @author Heinrich Mellmann
 */
public class PenDashed extends Pen {
    
    public PenDashed(float width, Color color) {
        super(color, new BasicStroke(20.0f,
                            BasicStroke.CAP_BUTT,
                            BasicStroke.JOIN_MITER,
                            100.0f, new float[]{100.0f}, 0.0f));
    }

    public PenDashed(String[] args) {
        this((Float.parseFloat(args[2])), Colors.parseColor(args[1]));
    }

}//end class Pen
