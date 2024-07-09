package de.naoth.rc.components.simspark.commands;

import java.util.Locale;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class BallCommand extends SimsparkCommand
{
    final private float x;
    final private float y;
    final private float z;

    final private Float vx;
    final private Float vy;
    final private Float vz;

    public BallCommand(float x, float y)
    {
        this(x, y, 0.2f);
    }

    public BallCommand(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
        this.vx = null;
        this.vy = null;
        this.vz = null;
    }

    public BallCommand(float x, float y, float z, float vx, float vy)
    {
        this(x, y, z, vx, vy, 0);
    }

    public BallCommand(float x, float y, float z, float vx, float vy, float vz)
    {
        this.x = x;
        this.y = y;
        this.z = z;
        this.vx = vx;
        this.vy = vy;
        this.vz = vz;
    }

    public static BallCommand centerCircle()
    {
        return new BallCommand(0, 0);
    }

    public static BallCommand rightLowerCorner()
    {
        return new BallCommand(4.5f, -3);
    }

    public static BallCommand rightUpperCorner()
    {
        return new BallCommand(4.5f, 3);
    }

    public static BallCommand leftLowerCorner()
    {
        return new BallCommand(-4.5f, -3);
    }

    public static BallCommand leftUpperCorner()
    {
        return new BallCommand(-4.5f, 3);
    }

    @Override
    public String getName()
    {
        return "Ball";
    }

    @Override
    public String getCommand()
    {
        return vx == null
                ? String.format(Locale.US, "(ball (pos %.2f %.2f %.2f))", x, y, z)
                : String.format(Locale.US, "(ball (pos %.2f %.2f %.2f)(vel %.2f %.2f %.2f))", x, y, z, vx, vy, vz);
    }
}
