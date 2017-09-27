/*
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
        return this.deviceID + " (" + this.mountPoint + ")";
    }
}
