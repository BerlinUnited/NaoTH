package de.naoth.rc.components.simspark.commands;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentSyncCommand extends SimsparkCommand
{
    @Override
    public String getName()
    {
        return "Sync";
    }

    @Override
    public String getCommand()
    {
        return "(syn)";
    }
}
