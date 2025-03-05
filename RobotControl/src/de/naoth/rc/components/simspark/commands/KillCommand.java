package de.naoth.rc.components.simspark.commands;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class KillCommand extends SimsparkCommand
{
    @Override
    public String getName()
    {
        return "Kill Simspark";
    }

    @Override
    public String getCommand()
    {
        return "(killsim)";
    }
}
