package de.naoth.rc.components.teamcommviewer;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.stream.JsonWriter;
import de.naoth.rc.components.teamcomm.TeamCommListener;
import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.dataformats.SPLMessage2018;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class TeamCommLogger implements Runnable, TeamCommListener
{
    
    @PluginImplementation
    public static class Plugin implements net.xeoh.plugins.base.Plugin {
        @InjectPlugin
        public static TeamCommManager teamcommManager;
    }//end Plugin

    /**
     * Thread, which writes the TeamCommMessages to the log file.
     */
    private Thread logger;
    
    /**
     * The log file where the TeamCommMessages gets written.
     */
    private File file;
    
    /**
     * The GsonBuilder used to convert TeamCommMessages to JSON.
     */
    private final Gson json;
    
    /**
     * The JSON writer.
     */
    private JsonWriter jw;
    
    /**
     * Queue for storing/buffering TeamCommMessages.
     */
    private final ConcurrentLinkedQueue<TeamCommMessage> logQueue = new ConcurrentLinkedQueue<>();
    
    /**
     * The state of the logger thread.
     * If set to false, any running logger thread will terminate.
     */
    public boolean running = true;
    
    /**
     * Indicates if the logger is ready to log.
     * This is set to true, if a writeable log file is set.s
     */
    private boolean isActive = false;

    /**
     * Configures the GsonBuilder and adds a ShutdownHook so that the log file gets closed correctly.
     */
    public TeamCommLogger() {
        // configure & create GsonBuilder
        this.json = new GsonBuilder()
            .serializeSpecialFloatingPointValues() // write NaN value without throwing error/warning
            .create();

        // closes the log file before exiting application
        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                stopLogging();
            }
        });
    }

    /**
     * Converts (to JSON) and writes the buffered TeamCommMessages to the log file.
     * Runs as separate thread.
     */
    @Override
    public void run() {
        while (running) {
            TeamCommMessage msg = logQueue.poll();
            if (msg != null) {
                try {
                    // NOTE: using the 'new' teamcomm format
                    //       'old' format: this.jw.jsonValue(this.json.toJson(msg));
                    jw.beginObject();
                    jw.name("timestamp").jsonValue(json.toJson(msg.timestamp));
                    jw.name("gameState").jsonValue(json.toJson(msg.gameState));
                    jw.name("address").jsonValue(json.toJson(msg.address));
                    jw.name("header").jsonValue(json.toJson("SPL "));
                    jw.name("version").jsonValue(json.toJson(SPLMessage2018.SPL_STANDARD_MESSAGE_STRUCT_VERSION));
                    jw.name("playerNum").jsonValue(json.toJson(msg.message.playerNum));
                    jw.name("teamNum").jsonValue(json.toJson(msg.message.teamNum));
                    jw.name("fallen").jsonValue(json.toJson(msg.message.fallen==1));
                    jw.name("pose").jsonValue(json.toJson(new float[]{msg.message.pose_x, msg.message.pose_y, msg.message.pose_a}));
                    jw.name("ballAge").jsonValue(json.toJson(msg.message.ballAge));
                    jw.name("ball").jsonValue(json.toJson(new float[]{msg.message.ball_x, msg.message.ball_y}));
                    jw.name("data").jsonValue(json.toJson(msg.message.data));
                    jw.endObject();
                } catch (IOException ex) {
                    Logger.getLogger(TeamCommLogger.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
    }

    /**
     * Sets the given file as log file.
     * Therefore any running logger is stopped, the file is checked if it is writeable
     * and the logger is set to "active".
     * @param f File, where the log should be written
     * @return true|false, if setting (new) log file was successfull
     */
    public boolean setLogFile(File f) {
        // make sure there is no open log file ..
        this.stopLogging();
        // check if file is writeable
        try {
            // create new log file
            f.createNewFile();
            new FileWriter(f).close(); // trigger exception (if couldn't write)
        } catch (IOException ex) {
            Logger.getLogger(TeamCommLogger.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
        // set logger "active"
        this.isActive = true;
        this.file = f;
        
        return true;
    }

    /**
     * Returns the log file.
     * @return File
     */
    public File getLogfile() {
        return this.file;
    }

    /**
     * Listener callback-method.
     * Appends the new TeamCommMessages to the log queue.
     * @param messages  TeamCommMessages
     */
    @Override
    public void newTeamCommMessages(List<TeamCommMessage> messages) {
        logQueue.addAll(messages);
    }

    /**
     * Returns if the logger is ready for logging, or not.
     * The logger is "active", when a log file is set.
     * @return true|false
     */
    public boolean isActive() {
        return this.isActive;
    }

    /**
     * Starts the logging process.
     * If not already started/active, a new log file is created & initialized and a listener to the
     * teamcomm messages is registered.
     * 
     * @return true|false, whether or not the logger thread could be startet
     */
    public boolean startLogging() {
        if(this.isActive) {
            try {
                // use JsonWriter to write the enclosing Json-Array ...
                this.jw = new JsonWriter(new FileWriter(this.file));
                // open "global"/"enclosing" Array
                this.jw.beginArray();
                this.resumeLogging();
                Plugin.teamcommManager.addListener(this);
                return true;
            } catch (IOException ex) {
                Logger.getLogger(TeamCommLogger.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        return this.isActive;
    }

    /**
     * Pauses the logger process.
     * Not teammessages are written to the log file.
     * 
     * @return true, if pausing was successfull, false otherwise
     */
    public boolean pauseLogging() {
        if(this.isActive && this.logger != null && this.logger.isAlive()) {
            try {
                this.running = false;
                this.logger.join();
                this.logger = null;
            } catch (InterruptedException ex) {
                Logger.getLogger(TeamCommLogger.class.getName()).log(Level.SEVERE, null, ex);
                return false;
            }
        }
        return true;
    }

    /**
     * Continues logging.
     * 
     * @return true, if re-start of the logger process was successfull, false otherwise
     */
    public boolean resumeLogging() {
        if(this.isActive) {
            this.running = true;
            this.logger = new Thread(this);
            this.logger.start();
            return true;
        }
        return false;
    }

    /**
     * Stops the logging process.
     * The log file is fully written and closed.
     * 
     * @return true, if stopping of the logger process was successfull, false otherwise
     */
    public boolean stopLogging() {
        Plugin.teamcommManager.removeListener(this);
        boolean result = this.pauseLogging();
        try {
            if(this.isActive && this.jw != null) {
                // flush buffer and close
                this.jw.endArray();
                this.jw.close();
            }
        } catch (IOException ex) {
            Logger.getLogger(TeamCommLogger.class.getName()).log(Level.SEVERE, null, ex);
        }
        this.isActive = false;
        return result;
    }
    
    /**
     * Returns the pending messages, which should be written to the log file.
     * 
     * @return number of pending messages
     */
    public int pendingLogQueueSize() {
        return logQueue.size();
    }
}
