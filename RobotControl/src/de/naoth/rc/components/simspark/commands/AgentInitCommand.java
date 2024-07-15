package de.naoth.rc.components.simspark.commands;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentInitCommand extends SimsparkCommand
{
    @Override
    public String getName()
    {
        return "Init";
    }

    @Override
    public String getCommand()
    {
        return "(init (unum 0)(teamname NaoTH))(syn)";
    }
}
