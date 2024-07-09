package de.naoth.rc.components.simspark.scene;

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
}
