/*
 * 
 */

package de.naoth.rc.manager;

import de.naoth.rc.RobotControl;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.MessageServer;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class SwingCommandExecutorImpl implements SwingCommandExecutor {

    private MessageServer messageServer;
    
    @Override
    public void executeCommand(ObjectListener<byte[]> listener, Command command) {
        if(this.messageServer != null) {
            this.messageServer.executeCommand(new SwingCommandListener(listener), command);
        } else {
            listener.errorOccured("MessageServer is not avaliable");
        }
    }
    
    @PluginLoaded
    public void setServer(RobotControl robotControl) {
        this.messageServer = robotControl.getMessageServer();
    }
}
