/*
 * 
 */

package de.naoth.rc.dialogs.behaviorviewer;

import java.util.ArrayList;

/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLBehaviorFrame {
    
    public ArrayList<XABSLBehavior.Symbol> inputSymbols = new ArrayList<XABSLBehavior.Symbol>();
    public ArrayList<XABSLBehavior.Symbol> outputSymbols = new ArrayList<XABSLBehavior.Symbol>();
    public ArrayList<XABSLAction> actions = new ArrayList<XABSLAction>();
    String agent = "bua";
}
