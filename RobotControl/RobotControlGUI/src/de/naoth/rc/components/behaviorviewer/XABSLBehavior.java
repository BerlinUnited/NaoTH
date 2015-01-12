/*
 * 
 */
package de.naoth.rc.components.behaviorviewer;

import de.naoth.rc.components.behaviorviewer.model.EnumType;
import de.naoth.rc.components.behaviorviewer.model.Symbol;
import de.naoth.rc.components.behaviorviewer.model.SymbolKey;
import de.naoth.rc.components.behaviorviewer.model.SymbolType;
import de.naoth.rc.components.behaviorviewer.model.Agent;
import de.naoth.rc.components.behaviorviewer.model.Option;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.TreeMap;

/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLBehavior {




    
    

    public final ArrayList<EnumType> enumerations = new ArrayList<>();
    public final ArrayList<Option> options = new ArrayList<>();
    public final ArrayList<Agent> agents = new ArrayList<>();
    
    public final Map<SymbolKey, Symbol> inputSymbols = new HashMap<>();
    public final Map<SymbolKey, Symbol> outputSymbols = new HashMap<>();
    
}
