/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package naoscp.tools.usb;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.List;

/**
 *
 * @author Robert Martin
 */
public abstract class USBStorageDeviceFinder {
    
    public List<USBStorageDevice> getUSBStorageDevices() {
        return new LinkedList<>();
    }
    
    static BufferedReader executeCommand(String command) {
        BufferedReader reader = null;

        Process p;
        try {
            p = Runtime.getRuntime().exec(command);
            p.waitFor();
            reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
        } catch (IOException | InterruptedException e) {
        }

        return reader;
    }
    
    public void unmount(USBStorageDevice usbStorageDevice) {
        
    }
 
}
