/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package naoscp.tools.usb;

/**
 *
 * @author Robert Martin
 */
public class USBStorageDevice {
    
    private String deviceID, mountPoint;
    
    public USBStorageDevice (String deviceID, String mountPoint) {
        this.deviceID = deviceID;
        this.mountPoint = mountPoint;
    }
    
    public String getMountPoint() {
        return this.mountPoint;
    }
    
    public String getDeviceID() {
        return this.deviceID;
    }
    
    public String toString() {
        return this.deviceID + " " + this.mountPoint;
    }
}
