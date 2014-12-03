/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.core.manager;

import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.ResponseListener;
import javax.swing.SwingUtilities;

/**
 *
 * @author Heinrich Mellmann
 */
public class SwingCommandListener implements ResponseListener {
    
    private final ObjectListener<byte[]> listener;
    public SwingCommandListener(ObjectListener<byte[]> listener)
    {
        this.listener = listener;
    }

    @Override
    public void handleResponse(final byte[] result, Command command) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                listener.newObjectReceived(result);
            }
        });
    }

    @Override
    public void handleError(final int code) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                listener.errorOccured("Robot detected an error: error code " + code);
            }
        });
    }
}
