/*
 *
 */

package de.naoth.rc.dialogs.behaviorviewer;

import de.naoth.rc.dialogs.behaviorviewer.model.Option;
import de.naoth.rc.dialogs.behaviorviewer.model.Symbol;
import java.util.ArrayList;


/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLAction {
    public static class OptionExecution extends XABSLAction{

        public int id;
        public Option option;
        public long timeOfExecution;

        public int activeState;
	public long stateTime;
	  
	ArrayList<XABSLAction> activeSubActions = new ArrayList<>();
    }
    
    public static class BasicBehaviorExecution extends XABSLAction{
        public String name;
    }
    
    public static class SymbolAssignement extends XABSLAction{
        public Symbol symbol;
    }
}
