package de.naoth.rc.components.simspark.commands;

import java.util.Locale;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentCommand extends SimsparkCommand
{
    final private String team;
    final private int number;
    final private float x;
    final private float y;
    final private float z = 0.2f;
    final private float r;

    public AgentCommand(String team, int number, float x, float y, float r)
    {
        this.team = team;
        this.number = number;
        this.x = x;
        this.y = y;
        this.r = r;
    }

    public static AgentCommand centerCircle(String team, int number)
    {
        return new AgentCommand(team, number, 0, 0, 90);
    }

    public static AgentCommand rightLowerCorner(String team, int number)
    {
        return new AgentCommand(team, number, 4.5f, -3, 90);
    }

    public static AgentCommand rightUpperCorner(String team, int number)
    {
        return new AgentCommand(team, number, 4.5f, 3, 90);
    }

    public static AgentCommand leftLowerCorner(String team, int number)
    {
        return new AgentCommand(team, number, -4.5f, -3, -90);
    }

    public static AgentCommand leftUpperCorner(String team, int number)
    {
        return new AgentCommand(team, number, -4.5f, 3, -90);
    }

    @Override
    public String getName()
    {
        return "Agent";
    }

    @Override
    public String getCommand()
    {
        return String.format(Locale.US, "(agent (unum %d) (team %s) (move %.2f %.2f %.2f %.2f))", number, team, x, y, z, r);
    }
}
