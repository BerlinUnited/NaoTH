/*
 */
package naoscp.tools.usb;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import naoscp.components.USBPanel;
import static naoscp.tools.usb.USBStorageDeviceManager.executeCommand;

/**
 *
 * @author Robert Martin
 */
public class OSXUSBStorageDeviceManager extends USBStorageDeviceManager {
    
    private static final String DEVICE_LIST_CMD = "system_profiler SPUSBDataType";
    
    private final static Pattern MOUNT_POINT_DEVICE_LIST_PATTERN = Pattern.compile("^.*Mount Point: (.+)$");
    
    @Override
    public List<USBStorageDevice> getUSBStorageDevices() {
       
        BufferedReader output = null;
        try {
            output = executeCommand(DEVICE_LIST_CMD);
        } catch (IOException ex) {
            Logger.getGlobal().log(Level.WARNING, "USBPanel: Error while executing " + DEVICE_LIST_CMD);
        } catch (InterruptedException ex) {
            
        }
        LinkedList<USBStorageDevice> usbDevices = new LinkedList<>();
        
        if(output == null) {
            return usbDevices;
        }
        
        String currentDevice = "USB Device";
        
        String outputLine;
        
        Matcher matcher;

        try {
            while ((outputLine = output.readLine()) != null) {               
                
                matcher = MOUNT_POINT_DEVICE_LIST_PATTERN.matcher(outputLine);
                
                if (matcher.matches()) {
                    String mountPoint = matcher.group(1); 
                   
                    usbDevices.add(new USBStorageDevice(currentDevice, mountPoint));
                }

            }
        } catch (IOException ex) {
            Logger.getLogger(USBPanel.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        return usbDevices;
    }
    
}
