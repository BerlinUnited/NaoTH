package de.naoth.rc.logplayer;

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
        try {
            System.loadLibrary("logsimulator");
        }
        catch (UnsatisfiedLinkError ex) {
            ex.printStackTrace();
        }
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
        
        private synchronized void notifyFrameChanged(int frame) {
            try{
            BlackBoard b = logSimulator.new BlackBoard();
            for(LogSimulatorActionListener l: listeners) {
                l.frameChanged(b, frame);
            }
            } catch(NullPointerException ex) {
                int x = 0;
            }
        }
        
        private synchronized void notifyFileOpenedChanged(String path) {
            BlackBoard b = logSimulator.new BlackBoard();
            for(LogSimulatorActionListener l: listeners) {
                l.logfileOpened(b, path);
            }
        }
        
        // singleton
        private LogSimulatorManager() {}
        private static final LogSimulatorManager logSimulatorManager = new LogSimulatorManager();
        public synchronized static LogSimulatorManager getInstance() {
            return logSimulatorManager;
        }
        
        public synchronized void addListener(LogSimulatorActionListener l) {
            listeners.add(l);
        }
        
        public synchronized void removeListener(LogSimulatorActionListener l) {
            listeners.remove(l);
        }
        
        public synchronized List<LogSimulatorActionListener> getListener() {
            return new ArrayList<>(listeners);
        }
        
        // open file
        public synchronized void openLogFile(String path) {
            this.logSimulator.openLogFile(path);
            notifyFileOpenedChanged(path);
        }
        
        // frame modifiers
        public synchronized void stepForward() {
            this.logSimulator.stepForward();
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        public synchronized void stepBack() {
            this.logSimulator.stepBack();
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        public synchronized void jumpToBegin() {
            this.logSimulator.jumpToBegin();
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        public synchronized void jumpToEnd() {
            this.logSimulator.jumpToEnd();
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        public synchronized void jumpTo(int position) {
            this.logSimulator.jumpTo(position);
            notifyFrameChanged(this.logSimulator.getCurrentFrame());
        }
        
        // getters
        public synchronized int getCurrentFrame() {
            return logSimulator.getCurrentFrame();
        }
        public synchronized int getMinFrame() {
            return logSimulator.getMinFrame();
        }
        public synchronized int getMaxFrame() {
            return logSimulator.getMaxFrame();
        }
    }
}
