/*
 * 
 */
package de.naoth.rc.components.behaviorviewer;

import de.naoth.rc.components.behaviorviewer.model.EnumType;
import de.naoth.rc.components.behaviorviewer.model.Symbol;
import de.naoth.rc.components.behaviorviewer.model.SymbolType;
import de.naoth.rc.components.behaviorviewer.model.Agent;
import de.naoth.rc.components.behaviorviewer.model.Option;
import de.naoth.rc.components.behaviorviewer.model.SymbolKey;
import de.naoth.rc.core.messages.Messages;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

/**
 * Parse the XABSL behavior from a protobuf serialized messages. 
 * @author Heinrich Mellmann
 */
public class XABSLProtoParser 
{
    // Internal behavior state.
    private XABSLBehavior behavior = null;

    /**
     * Parse the sparse behavior status based on the internal behavior state.
     * NOTE: the internal behavior has to be initialized.
     * 
     * @param status sparse behavior representation
     * @return parsed behavior frame
     */
    public XABSLBehaviorFrame parseSparse(Messages.BehaviorStateSparse status) 
    {
        XABSLBehaviorFrame frame = new XABSLBehaviorFrame(status.getFrameNumber());

        // parse input symbols
        ArrayList<Symbol> inputSymbols = parseSparse(behavior.inputSymbols, status.getInputSymbolList());
        for (Symbol s : inputSymbols) {
            frame.addInputSymbol(s);
        }

        // parse output symbols
        ArrayList<Symbol> outputSymbols = parseSparse(behavior.outputSymbols, status.getOutputSymbolList());
        for (Symbol s : outputSymbols) {
            frame.addOutputSymbol(s);
        }

        // parse active actions in this frame
        for (Messages.XABSLActionSparse a : status.getActiveRootActionsList()) {
            frame.actions.add(parse(a));
        }

        return frame;
    }

    public ArrayList<Symbol> parseSparse(
            Map<SymbolKey, Symbol> symbolDefinitions,
            Messages.SymbolValueList valueList) {
        
        // reserve enough spae to fit all symbols
        ArrayList<Symbol> result = new ArrayList<>(
                          valueList.getBooleanCount()
                        + valueList.getDecimalCount()
                        + valueList.getEnumeratedCount());
        
        // remember which symbols were explicitly set in this frame
        Set<SymbolKey> includedIDs = new HashSet<>();

        for (Messages.SymbolValueList.DoubleSymbol symbol : valueList.getDecimalList()) {
            SymbolKey key = new SymbolKey(SymbolType.DECIMAL, symbol.getId());
            Symbol symbolDefinition = symbolDefinitions.get(key);
            result.add( symbolDefinition.createDecimal( symbol.getValue()) );
            includedIDs.add(key);
        }

        for (Messages.SymbolValueList.BooleanSymbol symbol : valueList.getBooleanList()) {
            SymbolKey key = new SymbolKey(SymbolType.BOOL, symbol.getId());
            Symbol symbolDefinition = symbolDefinitions.get(key);
            result.add( symbolDefinition.createBoolean( symbol.getValue()) );
            includedIDs.add(key);
        }

        for (Messages.SymbolValueList.EnumSymbol symbol : valueList.getEnumeratedList()) {
            SymbolKey key = new SymbolKey(SymbolType.ENUM, symbol.getId());
            Symbol symbolDefinition = symbolDefinitions.get(key);
            result.add( symbolDefinition.createEnum( symbol.getValue()) );
            includedIDs.add(key);
        }
        
        
        // also add all symbols that are in the existing list but have not been
        // included in the sparse message
        //
        // TODO: don't copy all unchanged default values in each frame.
        // Frame should have a reference to XABSLBehavior and have direct access 
        // to the default values if needed.
        for(Map.Entry<SymbolKey, Symbol> entry : symbolDefinitions.entrySet())
        {
            if(!includedIDs.contains(entry.getKey()))
            {
                result.add(entry.getValue());
            }
        }
        
        return result;
    }

    public XABSLAction parse(Messages.XABSLActionSparse action_msg) 
    {
        // TODO: what does this mean? Exception?
        if (null == action_msg.getType()) {
            return null;
        }
            
        switch (action_msg.getType()) 
        {
            case Option:
            {
                Messages.XABSLActiveOptionSparse option_msg = action_msg.getOption();
                XABSLAction.OptionExecution a = new XABSLAction.OptionExecution();
                
                a.activeState = option_msg.getActiveState();
                a.timeOfExecution = option_msg.getTimeOfExecution();
                a.stateTime = option_msg.getStateTime();
                a.id = option_msg.getId();
                
                Option protoOption = this.behavior.options.get(a.id);
                a.option = new Option(protoOption); // make a shallow copy
                
                // parse Parameters
                Iterator<Double> iterDecimal = option_msg.getDecimalParametersList().iterator();
                Iterator<Boolean> iterBoolean = option_msg.getBooleanParametersList().iterator();
                Iterator<Integer> iterEnum = option_msg.getEnumeratedParametersList().iterator();

                for(int i = 0; i < a.option.parameters.size(); i++)
                {
                    Symbol symbol = a.option.parameters.get(i);
                    
                    if(symbol.getDataType() == SymbolType.DECIMAL) {
                        if(iterDecimal.hasNext()) {
                            double v = iterDecimal.next();
                            //Symbol.Decimal s = (Symbol.Decimal)symbol;
                            a.option.parameters.set(i, symbol.createDecimal(v));
                        } else {
                            // todo: exception
                        }
                    } else if(symbol.getDataType() == SymbolType.BOOL) {
                        if(iterBoolean.hasNext()) {
                            boolean v = iterBoolean.next();
                            //Symbol.Boolean s = (Symbol.Boolean)symbol;
                            a.option.parameters.set(i, symbol.createBoolean(v));
                        } else {
                            // todo: exception
                        }
                    } else if(symbol.getDataType() == SymbolType.ENUM) {
                        if(iterEnum.hasNext()) {
                            int v = iterEnum.next();
                            //Symbol.Enum s = (Symbol.Enum)symbol;
                            a.option.parameters.set(i, symbol.createEnum(v));
                        } else {
                            // todo: exception
                        }
                    } else {
                        // todo: exception
                    }
                }
                
                for (Messages.XABSLActionSparse sa : option_msg.getActiveSubActionsList()) {
                    a.activeSubActions.add(parse(sa));
                }
                
                return a;
            }
            case SymbolAssignment:
            {
                Symbol symbol = parse_output_symbol(action_msg.getSymbol());
                return new XABSLAction.SymbolAssignment(symbol);
            }
            case BasicBehavior:
            {
                return new XABSLAction.BasicBehaviorExecution("Some Basic Behavior (not supported yet)");
            }
            default:
                // this should never happen
                // TODO: unknown type? Exception?
                return null;
        }
    }
    
    /**
     * NOTE: similar to @parse_parameters(), but works with existing output symbols.
     * I.e., it checks if the symbol really exists.
     * @param s_msg
     * @return 
     */
    private Symbol parse_output_symbol(Messages.XABSLSymbol s_msg) 
    {
        // find the definition of the output symbol
        SymbolKey key = new SymbolKey(parse(s_msg.getType()), s_msg.getId());
        Symbol symbol = behavior.outputSymbols.get(key);
        
        switch (s_msg.getType()) {
            case Decimal:
                return symbol.createDecimal(s_msg.getDecimalValue());
            case Boolean:
                return symbol.createBoolean(s_msg.getBoolValue());
            case Enum:
                return symbol.createEnum(s_msg.getEnumValue());
        }
        
        // this should never happen
        return null;
    }
    
    private SymbolType parse(Messages.XABSLSymbol.SymbolType type) {
        switch(type)
        {
            case Boolean: return SymbolType.BOOL;
            case Enum: return SymbolType.ENUM;
            case Decimal: return SymbolType.DECIMAL;
            default: return SymbolType.VOID;
        }
    }

    /**
     * Parse a complete XABSL behavior from a protobuf message and initialize 
     * or replace the internal behavior state.
     * @param status behavior representation
     * @return 
     */
    public XABSLBehavior parseComplete(Messages.BehaviorStateComplete status) {
        this.behavior = new XABSLBehavior();

        // parse the enumerations first, so they are available when symbols are parsed
        for (int i = 0; i < status.getEnumerationsCount(); i++) {
            behavior.enumerations.add(i, parse(status.getEnumerations(i)));
        }
        for (int i = 0; i < status.getOptionsCount(); i++) {
            behavior.options.add(i, parse(status.getOptions(i)));
        }
        for (int i = 0; i < status.getAgentsCount(); i++) {
            behavior.agents.add(i, parse(status.getAgents(i)));
        }

        behavior.inputSymbols.putAll(parse(status.getInputSymbolList()));
        behavior.outputSymbols.putAll(parse(status.getOutputSymbolList()));

        return this.behavior;
    }

    public Map<SymbolKey, Symbol> parse(Messages.SymbolValueList valueList) {
        Map<SymbolKey, Symbol> result = new HashMap<>();

        for (Messages.SymbolValueList.DoubleSymbol s_msg : valueList.getDecimalList()) {
            result.put(new SymbolKey(SymbolType.DECIMAL, s_msg.getId()),
                    new Symbol.Decimal(s_msg.getName(), s_msg.getValue()));
        }

        for (Messages.SymbolValueList.BooleanSymbol s_msg : valueList.getBooleanList()) {
            result.put(new SymbolKey(SymbolType.BOOL, s_msg.getId()),
                    new Symbol.Boolean(s_msg.getName(),
                            s_msg.getValue()));
        }

        for (Messages.SymbolValueList.EnumSymbol s_msg : valueList.getEnumeratedList()) {
            EnumType enumType = this.behavior.enumerations.get(s_msg.getTypeId());
            result.put(new SymbolKey(SymbolType.ENUM, s_msg.getId()),
                    new Symbol.Enum(s_msg.getName(), enumType, s_msg.getValue()));
        }

        return result;
    }

    private Option parse(Messages.BehaviorStateComplete.Option option_msg) {
        Option option = new Option(option_msg.getName());

        for (int j = 0; j < option_msg.getParametersCount(); j++) {
            option.parameters.add(j, parse_parameter(option_msg.getParameters(j)));
        }

        for (int j = 0; j < option_msg.getStatesCount(); j++) {
            option.states.add(j, parse(option_msg.getStates(j)));
        }

        return option;
    }

    private Option.State parse(Messages.BehaviorStateComplete.Option.State state_msg) {
        return new Option.State(state_msg.getName(), state_msg.getTarget());
    }

    private Symbol parse_parameter(Messages.XABSLSymbol s_msg) 
    {
        switch (s_msg.getType()) {
            case Decimal:
                return new Symbol.Decimal(s_msg.getName(), s_msg.getDecimalValue());
            case Boolean:
                return new Symbol.Boolean(s_msg.getName(), s_msg.getBoolValue());
            case Enum:
                // find athe enum definition based on the type id and generate a value
                EnumType enumType = this.behavior.enumerations.get(s_msg.getEnumTypeId());
                return new Symbol.Enum(s_msg.getName(), enumType, s_msg.getEnumValue());
        }
        
        // this should never occur
        return null;
    }

    private Agent parse(Messages.BehaviorStateComplete.Agent agent_msg) {
        return new Agent( agent_msg.getName(), agent_msg.getRootOption() );
    }

    private EnumType parse(Messages.BehaviorStateComplete.EnumType enum_msg) {
        EnumType enumeration = new EnumType(enum_msg.getName());
        for (Messages.BehaviorStateComplete.EnumType.Element msgElement : enum_msg.getElementsList()) 
        {
            EnumType.Element e = new EnumType.Element(
                msgElement.getValue(),
                // remove the preceeding type name, e.g., motion.type.walk => walk
                msgElement.getName().replace(enumeration.name+".", ""));
            
            enumeration.elements.put(e.value, e);
        }
        return enumeration;
    }

    /*
    private EnumType.Element parse(Messages.BehaviorStateComplete.EnumType.Element element_msg) {
        EnumType.Element element = new EnumType.Element(
            (int) element_msg.getValue(),
            element_msg.getName());
        
        return element;
    }*/
}
