/*
 * 
 */

package de.naoth.rc.manager;

import de.naoth.rc.core.manager.ManagerPlugin;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.dialogs.drawings.DrawingsContainer;
import java.util.Map;

/**
 *
 * @author Heinrich Mellmann
 */
public interface TeamCommDrawingManager extends ManagerPlugin<DrawingsContainer>
{
  public void handleSPLMessages(Map<String, SPLMessage> messages);
}
