
package de.naoth.rc.dataformats;

import java.util.HashMap;
import java.util.List;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkState extends HashMap<String, Object>{

    public SimsparkState() {
        put("time", null);
        put("play_mode", null);
        put("score_left", null);
        put("score_right", null);
        put("half", null);
        put("FieldLength", null);
        put("FieldWidth", null);
        put("FieldHeight", null);
        put("GoalWidth", null);
        put("GoalDepth", null);
        put("GoalHeight", null);
        put("BorderSize", null);
        put("FreeKickDistance", null);
        put("WaitBeforeKickOff", null);
        put("AgentRadius", null);
        put("BallRadius", null);
        put("BallMass", null);
        put("RuleGoalPauseTime", null);
        put("RuleKickInPauseTime", null);
        put("RuleHalfTime", null);
        put("play_modes", null);
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
                replace(key, Integer.parseInt((String) value));
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
                replace(key, Double.parseDouble((String)value));
                break;
            case "play_modes":
                replace(key, (value instanceof List ? value:null));
                break;
        }
    }

    @Override
    public Object get(Object key) {
        if(key == "play_mode") {
            return getPlayMode();
        }
        return super.get(key);
    }

    public String getPlayMode() {
        if(super.get("play_mode") != null && super.get("play_modes") != null){
            if(((List<String>)super.get("play_modes")).size() > (int)super.get("play_mode")) {
                return ((List<String>)super.get("play_modes")).get((int)super.get("play_mode"));
            }
        }
        return super.get("play_mode")+"";
    }
}
