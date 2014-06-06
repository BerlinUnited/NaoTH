/*
 * 
 */

package de.naoth.rc.dialogs.behaviorviewer;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLBehaviorFrame 
{

    public enum SymbolIOType {input, output, unknown}
    
    private Map<String, XABSLBehavior.Symbol> inputSymbols = new LinkedHashMap<>();
    private Map<String, XABSLBehavior.Symbol> outputSymbols = new LinkedHashMap<>();
    public ArrayList<XABSLAction> actions = new ArrayList<>();
    
    public final int frameNumber;
    
    public XABSLBehaviorFrame(int frameNumber)
    {
        this.frameNumber = frameNumber;
    }
    
    public void addInputSymbol(XABSLBehavior.Symbol symbol)
    {
        if(symbol != null)
        {
            inputSymbols.put(symbol.name, symbol);
        }
    }
    
    public void addOutputSymbol(XABSLBehavior.Symbol symbol)
    {
        if(symbol != null)
        {
            outputSymbols.put(symbol.name, symbol);
        }
    }
    
    public XABSLBehavior.Symbol getSymbolByName(String name)
    {
        XABSLBehavior.Symbol result;
        result = outputSymbols.get(name);
        if(result == null)
        {
            result = inputSymbols.get(name);
        }
        
        // get the default value from the behavior
        if(result == null)
        {
            
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
