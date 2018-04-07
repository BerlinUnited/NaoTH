
package de.naoth.rc.dataformats;

import java.util.Collections;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Representation of the simspark state.
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkState
{
    /** Container holding the state of the connected simspark instance */
    public final ConcurrentHashMap<String, Object> data = new ConcurrentHashMap<>();
    
    /** Indicator, whether the state was already updated or not. */
    public boolean hasBeenUpdated = false;
    
    /**
     * Constructor.
     * Initializes this simspark state with known fields.
     */
    public SimsparkState() {
        data.put("time", "");
        data.put("play_mode", "");
        data.put("score_left", "");
        data.put("score_right", "");
        data.put("half", "");
        data.put("FieldLength", "");
        data.put("FieldWidth", "");
        data.put("FieldHeight", "");
        data.put("GoalWidth", "");
        data.put("GoalDepth", "");
        data.put("GoalHeight", "");
        data.put("BorderSize", "");
        data.put("FreeKickDistance", "");
        data.put("WaitBeforeKickOff", "");
        data.put("AgentRadius", "");
        data.put("BallRadius", "");
        data.put("BallMass", "");
        data.put("RuleGoalPauseTime", "");
        data.put("RuleKickInPauseTime", "");
        data.put("RuleHalfTime", "");
        data.put("play_modes", "");
        data.put("messages", "");
    }
    
    /**
     * Updates a field of this simspark state representation with the given value.
     * 
     * @param key the field which should be updated/set
     * @param value the value of the updated field
     */
    public void set(String key, Object value) {
        switch(key) {
            case "FieldLength":
            case "FieldWidth":
            case "FieldHeight":
            case "WaitBeforeKickOff":
            case "RuleGoalPauseTime":
            case "RuleKickInPauseTime":
            case "RuleHalfTime":
            case "half":
            case "score_left":
            case "score_right":
            case "play_mode":
                data.replace(key, Integer.parseInt((String) value));
                break;
            case "time":
            case "GoalWidth":
            case "GoalDepth":
            case "GoalHeight":
            case "BorderSize":
            case "FreeKickDistance":
            case "AgentRadius":
            case "BallRadius":
            case "BallMass":
                data.replace(key, Double.parseDouble((String)value));
                break;
            case "play_modes":
            case "messages":
                data.replace(key, (value instanceof List ? value:Collections.emptyList()));
                break;
        }
        hasBeenUpdated = true;
    }

    /**
     * Returns the value of this state field.
     * 
     * @param key the state field
     * @return the value of the state field
     */
    public Object get(Object key) {
        if(key == "play_mode") {
            return getPlayMode();
        }
        return data.get(key);
    }

    /**
     * Returns the currently active play mode as string.
     * 
     * @return the active play mode
     */
    public String getPlayMode() {
        Object playmode = data.get("play_mode");
        Object playmodes = data.get("play_modes");
        // safety checks
        if(playmode != null && playmode instanceof Integer && 
           playmodes != null && playmodes instanceof List) {
            List<String> playmodesList = (List<String>)playmodes;
            Integer playmodeNum = (Integer) playmode;
            // return "known" playmode
            if(playmodesList.size() > playmodeNum) {
                return playmodesList.get(playmodeNum);
            }
            // return "unkown" num
            return playmodeNum.toString();
        }
        return "unknown";
    }
}
