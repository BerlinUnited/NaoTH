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

/**
 *
 * @author Robert Matin
 */
public class LinuxUSBStorageDeviceManager extends USBStorageDeviceManager{
    
    private final static String USB_PROPTERTY = "ID_USB_DRIVER=usb-storage";
    private final static String PROPERTY_CHECK_CMD = "udevadm info -q property -n ";
    
    private final static String DEVICE_LIST_CMD = "df ";
    private final static Pattern DEVICE_LIST_PATTERN = Pattern.compile("^(\\/[^ ]+)[^%]+%[ ]+(.+)$");

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

        String outputLine;

        try {
            while ((outputLine = output.readLine()) != null) {

                Matcher matcher = DEVICE_LIST_PATTERN.matcher(outputLine);

                if (matcher.matches()) {
                    String device = matcher.group(1);
                    String mountPoint = matcher.group(2);
                    
                    BufferedReader out = null;
                    try {
                        out = executeCommand(PROPERTY_CHECK_CMD + device);
                    } catch (IOException ex) {
                        Logger.getGlobal().log(Level.WARNING, "USBPanel: Error while executing " + PROPERTY_CHECK_CMD + device);
                    } catch (InterruptedException ex) {
                        
                    }
                    
                    if(out == null) {
                        return usbDevices;
                    }
                  
                    
                    String property;
                    while ((property = out.readLine()) != null) {
                        if (property.matches(USB_PROPTERTY)) {
                            usbDevices.add(new USBStorageDevice(device, mountPoint));
                            break;
                        }
                    }
                }

            }
        } catch (IOException ex) {
            Logger.getLogger(USBPanel.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        return usbDevices;
    }
    
    @Override
    public void unmount(USBStorageDevice usbStorageDevice) {        
        try {
            executeCommand("umount " + usbStorageDevice.getDeviceID());
        } catch (IOException ex) {
            Logger.getGlobal().log(Level.WARNING, "USBPanel: Error while executing umount " + usbStorageDevice.getDeviceID());
        } catch (InterruptedException ex) {
            
        }
        
        // Check if device is really unmounted
        for (USBStorageDevice storageDevice : this.getUSBStorageDevices()) {
            if(storageDevice.getDeviceID().matches(usbStorageDevice.getDeviceID())){
                Logger.getGlobal().log(Level.WARNING, "USBPanel: Couldn't unmout " + usbStorageDevice.toString());
                return;
            }
        }
        
        Logger.getGlobal().log(Level.INFO, usbStorageDevice.toString() + " successfully unmounted!");
       
    }
    
}
