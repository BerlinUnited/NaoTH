package de.naoth.rc.components.simspark.commands;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentSceneCommand extends SimsparkCommand
{
    @Override
    public String getName()
    {
        return "Scene (Init)";
    }

    @Override
    public String getCommand()
    {
        return "(scene rsg/agent/nao/nao.rsg 0)(syn)";
    }
}
