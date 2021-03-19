package de.naoth.rc.logmanager;

import java.util.Set;

/**
 * @author thomas
 */
public interface BlackBoard
{
    public Set<String> getNames();

    public boolean has(String name);

    public LogDataFrame get(String name);
}
