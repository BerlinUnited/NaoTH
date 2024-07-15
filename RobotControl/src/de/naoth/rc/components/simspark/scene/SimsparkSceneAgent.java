package de.naoth.rc.components.simspark.scene;

import de.naoth.rc.drawings.DrawingCollection;
import de.naoth.rc.drawings.Pen;
import de.naoth.rc.drawings.Robot;
import de.naoth.rc.drawings.Text;
import java.awt.Color;
import java.awt.Font;

/**
 * Representation of a simspark agent.
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkSceneAgent extends SimsparkSceneObject
{
    public String team;
    public int number;
    public float x;
    public float y;
    public float z;
    public float r;

    public SimsparkSceneAgent(int sceneIndex, String team, int number, float x, float y, float z, float r)
    {
        super(sceneIndex);

        this.team = team;
        this.number = number;
        this.x = x;
        this.y = y;
        this.z = z;
        this.r = r;
    }

    public void update(float x, float y, float z, float r)
    {
        this.x = x;
        this.y = y;
        this.z = z;
        this.r = r;
    }

    @Override
    public String toString()
    {
        return String.format("Player %s#%d: %.2f, %.2f, %.2f, %.2f", team, number, x, y, z, r);
    }

    @Override
    public void draw(DrawingCollection drawings)
    {
        // do not draw agents outside the field
        if (Math.abs(x) > 5 || Math.abs(y) > 4)
        {
            return;
        }

        // convert simspark coordinates in m to mm
        int globalX = (int) (x * 1000);
        int globalY = (int) (y * 1000);
        Color color = team.equals("Left") ? Color.BLUE : Color.RED;

        // robot
        drawings.add(new Pen(1.0f, color));
        drawings.add(new Robot(globalX, globalY, r));

        // add player number
        Font numberFont = new Font("Courier New", Font.PLAIN | Font.CENTER_BASELINE, 250);
        drawings.add(new Text(globalX, globalY + 250, 0, Integer.toString(number), numberFont));
    }
}
