package de.naoth.rc;

/**
 *
 * @author Heinrich
 */
public class LogSimulator {
    /*
    static {
        // Load native library at runtime
        System.loadLibrary("logsimulator");
    }
    */
    
    public LogSimulator() {
        System.loadLibrary("logsimulator");
    }

    public native void stepForward();
    public native void stepBack();
    public native void jumpToBegin();
    public native void jumpToEnd();
    public native void jumpTo(int position);
    public native void openLogFile(String path);

    public native int getCurrentFrame();
    public native int getMinFrame();
    public native int getMaxFrame();
}
