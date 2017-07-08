
package de.naoth.rc.dataformats;

import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkState {
    private final ConcurrentHashMap<String, Object> data = new ConcurrentHashMap<>();
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
                data.replace(key, (value instanceof List ? value:null));
                break;
        }
    }

    public Object get(Object key) {
        if(key == "play_mode") {
            return getPlayMode();
        }
        return data.get(key);
    }

    public String getPlayMode() {
        Integer playmode = (Integer) data.get("play_mode");
        List<String> playmodes = (List<String>) data.get("play_modes");
        if(playmode != null && playmodes != null){
            if(playmodes.size() > playmode) {
                return playmodes.get(playmode);
            }
        }
        return playmode.toString();
    }
}
