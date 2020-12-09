/*
 * 
 */

package de.naoth.rc.manager;

import de.naoth.rc.core.manager.ManagerPlugin;
import de.naoth.rc.dataformats.ModuleConfiguration;

/**
 *
 * @author Heinrich Mellmann
 */
public interface ModuleConfigurationManager extends ManagerPlugin<ModuleConfiguration>
{
    abstract void setModuleOwner(String name);
}