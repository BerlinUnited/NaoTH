/*
 * 
 */

package de.naoth.rc.components.behaviorviewer;

import de.naoth.rc.components.behaviorviewer.model.Symbol;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLBehaviorFrame 
{
    public final XABSLBehavior behavior;
            
    public enum SymbolIOType {input, output, unknown}
    
    private final Map<String, Symbol> inputSymbols = new LinkedHashMap<>();
    private final Map<String, Symbol> outputSymbols = new LinkedHashMap<>();
    public ArrayList<XABSLAction> actions = new ArrayList<>();
    
    public final int frameNumber;
    
    public XABSLBehaviorFrame(int frameNumber)
    {
        this(frameNumber, null);
    }
    
    public XABSLBehaviorFrame(int frameNumber, XABSLBehavior behavior)
    {
        this.frameNumber = frameNumber;
        this.behavior = behavior;
    }
    
    public void addInputSymbol(Symbol symbol)
    {
        if(symbol != null)
        {
            inputSymbols.put(symbol.name, symbol);
        }
    }
    
    public void addOutputSymbol(Symbol symbol)
    {
        if(symbol != null)
        {
            outputSymbols.put(symbol.name, symbol);
        }
    }
    
    public Symbol getSymbolByName(String name)
    {
        Symbol result = outputSymbols.get(name);
        
        if(result == null)
        {
            result = inputSymbols.get(name);
        }
        
        // get the default value from the behavior
        // TODO: at the moment 
        if(result == null && behavior != null)
        {
            // TODO:???
        }
        
        return result;
    }
    
    public SymbolIOType getSymbolIOType(String name)
    {
        if(outputSymbols.containsKey(name))
        {
            return SymbolIOType.output;
        }
        else if(inputSymbols.containsKey(name))
        {
            return SymbolIOType.input;
        }
        else
        {
            return SymbolIOType.unknown;
        }
    }
    
}
