/*
 * 
 */
package de.naoth.rc.drawings;

import java.awt.Graphics2D;
import java.util.ArrayList;

/**
 *
 * @author Heinrich Mellmann
 */
public class DrawingCollection implements Drawable {

    protected final ArrayList<Drawable> drawables = new ArrayList<>();

    public void add(Drawable d) {
        if(d != null) {
            this.drawables.add(d);
        }
    }
    
    public void clear() {
        drawables.clear();
    }

    @Override
    public void draw(Graphics2D g2d) {
        // NOTE: for unknown reason, sometimes an ConcurrentModificationException is thrown with the
        //       lambda expression of this loop!
        for (Drawable d : drawables) {
            d.draw(g2d);
        }
    }
    
    public boolean isEmpty() {
        return drawables.isEmpty();
    }

}//end DrawingCollection
