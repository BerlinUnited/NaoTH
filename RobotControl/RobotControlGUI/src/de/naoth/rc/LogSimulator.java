package de.naoth.rc;

import java.util.ArrayList;
import java.util.List;

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

    private native byte[] getRepresentation(String name);
    
    public class BlackBoard
    {
        public byte[] getRepresentation(String name) {
            return LogSimulator.this.getRepresentation(name);
        }
        
        public int getCurrentFrame() {
            return LogSimulator.this.getCurrentFrame();
        }
        public int getMinFrame() {
            return LogSimulator.this.getMinFrame();
        }
        public int getMaxFrame() {
            return LogSimulator.this.getMaxFrame();
        }
        
        public String getLogPath() {
            return null;
        }
    }
    
    public static interface LogSimulatorActionListener
    {
        abstract void frameChanged(BlackBoard b, int frameNumber);
        abstract void logfileOpened(BlackBoard b, String path);
    }
    
    public static class LogSimulatorManager
    {
        private final List<LogSimulatorActionListener> listeners = new ArrayList<>();
        private final LogSimulator logSimulator = new LogSimulator();
        
        private void notifyFrameChanged(int frame) {
            BlackBoard b = logSimulator.new BlackBoard();
            for(LogSimulatorActionListener l: listeners) {
                l.frameChanged(b, frame);
            }
        }
        
        private void notifyFileOpenedChanged(String path) {
            BlackBoard b = logSimulator.new BlackBoard();
            for(LogSimulatorActionListener l: listeners) {
                l.logfileOpened(b, path);
            }
        }
        
        // singleton
        private LogSimulatorManager() {}
        private static final LogSimulatorManager logSimulatorManager = new LogSimulatorManager();
        public static LogSimulatorManager getInstance() {
            return logSimulatorManager;
        }
        
        public void addListener(LogSimulatorActionListener l) {
            listeners.add(l);
        }
        
        public void removeListener(LogSimulatorActionListener l) {
            listeners.remove(l);
        }
        
        // open file
        public void openLogFile(String path) {
            this.logSimulator.openLogFile(path);
            notifyFileOpenedChanged(path);
        }
        
        // frame modifiers
        public void stepForward() {
            this.logSimulator.stepForward();
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        public void stepBack() {
            this.logSimulator.stepBack();
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        public void jumpToBegin() {
            this.logSimulator.jumpToBegin();
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        public void jumpToEnd() {
            this.logSimulator.jumpToEnd();
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        public void jumpTo(int position) {
            this.logSimulator.jumpTo(position);
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        
        // getters
        public int getCurrentFrame() {
            return logSimulator.getCurrentFrame();
        }
        public int getMinFrame() {
            return logSimulator.getMinFrame();
        }
        public int getMaxFrame() {
            return logSimulator.getMaxFrame();
        }
    }
}
