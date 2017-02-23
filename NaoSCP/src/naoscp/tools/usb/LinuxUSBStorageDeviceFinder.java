/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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
public class LinuxUSBStorageDeviceFinder extends USBStorageDeviceFinder{
    
    private final static String USB_PROPTERTY = "ID_USB_DRIVER=usb-storage";
    private final static String PROPERTY_CHECK_CMD = "udevadm info -q property -n ";
    
    private final static String DEVICE_LIST_CMD = "df ";
    private final static Pattern DEVICE_LIST_PATTERN = Pattern.compile("^(\\/[^ ]+)[^%]+%[ ]+(.+)$");

    @Override
    public List<USBStorageDevice> getUSBStorageDevices() {
        
        
        BufferedReader output = executeCommand(DEVICE_LIST_CMD);

        LinkedList<USBStorageDevice> usbDevices = new LinkedList<>();

        String outputLine;

        try {
            while ((outputLine = output.readLine()) != null) {

                Matcher matcher = DEVICE_LIST_PATTERN.matcher(outputLine);

                if (matcher.matches()) {
                    String device = matcher.group(1);
                    String mountPoint = matcher.group(2);
                    
                    BufferedReader out = executeCommand(PROPERTY_CHECK_CMD + device);
                    
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
        executeCommand("umount " + usbStorageDevice.getDeviceID());
    }
    
}
