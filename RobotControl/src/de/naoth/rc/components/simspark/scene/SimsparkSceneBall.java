package de.naoth.rc.components.simspark.scene;

import de.naoth.rc.drawings.Circle;
import de.naoth.rc.drawings.DrawingCollection;
import de.naoth.rc.drawings.FillOval;
import de.naoth.rc.drawings.Pen;
import java.awt.Color;

/**
 * Representation of a ball in simspark.
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkSceneBall extends SimsparkSceneObject
{
    public float x;
    public float y;
    public float z;

    public SimsparkSceneBall(int sceneIndex, float x, float y, float z)
    {
        super(sceneIndex);

        this.x = x;
        this.y = y;
        this.z = z;
    }

    public void update(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    @Override
    public String toString()
    {
        return String.format("Ball: %.2f, %.2f, %.2f", x, y, z);
    }

    @Override
    public void draw(DrawingCollection drawings)
    {
        // do not draw ball outside the field
        if (Math.abs(x) > 5 || Math.abs(y) > 4)
        {
            return;
        }

        // convert simspark coordinates in m to mm
        int globalX = (int) (x * 1000);
        int globalY = (int) (y * 1000);

        drawings.add(new Pen(1, Color.orange));
        drawings.add(new FillOval(globalX, globalY, 65, 65));

        // add a surrounding black circle so the ball is easier to see
        drawings.add(new Pen(1, Color.BLACK));
        drawings.add(new Circle((int) globalX, (int) globalY, 65));
    }
}
