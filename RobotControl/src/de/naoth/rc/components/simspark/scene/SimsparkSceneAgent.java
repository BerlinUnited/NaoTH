package de.naoth.rc.components.simspark.scene;

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
}
