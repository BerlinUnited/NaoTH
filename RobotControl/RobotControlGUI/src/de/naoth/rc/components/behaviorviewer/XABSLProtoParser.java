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
import static de.naoth.rc.components.behaviorviewer.model.SymbolType.BOOL;
import static de.naoth.rc.components.behaviorviewer.model.SymbolType.DECIMAL;
import static de.naoth.rc.components.behaviorviewer.model.SymbolType.ENUM;
import static de.naoth.rc.components.behaviorviewer.model.SymbolType.VOID;
import de.naoth.rc.messages.Messages;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;


/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLProtoParser {

    private XABSLBehavior behavior = null;

    public XABSLBehaviorFrame parseSparse(Messages.BehaviorStateSparse status) {
        XABSLBehaviorFrame frame = new XABSLBehaviorFrame(status.getFrameNumber());

        ArrayList<Symbol> inputSymbols = XABSLProtoParser.this.parseSparse(
                behavior.inputSymbols,
                status.getInputSymbolList());
        for (Symbol s : inputSymbols) {
            frame.addInputSymbol(s);
        }

        ArrayList<Symbol> outputSymbols = XABSLProtoParser.this.parseSparse(
                behavior.outputSymbols,
                status.getOutputSymbolList());
        for (Symbol s : outputSymbols) {
            frame.addOutputSymbol(s);
        }

        for (Messages.XABSLActionSparse a : status.getActiveRootActionsList()) {
            frame.actions.add(parse(a));
        }

        return frame;
    }

    public ArrayList<Symbol> parseSparse(
            Map<SymbolKey, Symbol> existing,
            Messages.SymbolValueList valueList) {
        
        ArrayList<Symbol> result
                = new ArrayList<>(valueList.getBooleanCount()
                        + valueList.getDecimalCount()
                        + valueList.getEnumeratedCount());
        
        Set<SymbolKey> includedIDs = new HashSet<>();

        for (Messages.SymbolValueList.DoubleSymbol symbol : valueList.getDecimalList()) {
            SymbolKey key = new SymbolKey(SymbolType.DECIMAL, symbol.getId());
            Symbol s = existing.get(key);
            result.add(parse(s, symbol));
            includedIDs.add(key);
        }

        for (Messages.SymbolValueList.BooleanSymbol symbol : valueList.getBooleanList()) {
            SymbolKey key = new SymbolKey(SymbolType.BOOL, symbol.getId());
            Symbol s = existing.get(key);
            result.add(parse(s, symbol));
            includedIDs.add(key);
        }

        for (Messages.SymbolValueList.EnumSymbol symbol : valueList.getEnumeratedList()) {
            SymbolKey key = new SymbolKey(SymbolType.ENUM, symbol.getId());
            Symbol s = existing.get(key);
            result.add(parse((Symbol.Enum) s, symbol));
            includedIDs.add(key);
        }
        
        // also add all symbols that are in the existing list but have not been
        // included in the sparse message
        for(Map.Entry<SymbolKey, Symbol> entry : existing.entrySet())
        {
            if(!includedIDs.contains(entry.getKey()))
            {
                result.add(entry.getValue());
            }
        }
        
        return result;
    }

    public XABSLAction parse(Messages.XABSLActionSparse action_msg) {
        if (action_msg.getType() == Messages.XABSLActionSparse.ActionType.Option) {
            Messages.XABSLActiveOptionSparse option_msg = action_msg.getOption();
            XABSLAction.OptionExecution a = new XABSLAction.OptionExecution();

            a.activeState = option_msg.getActiveState();
            a.timeOfExecution = option_msg.getTimeOfExecution();
            a.stateTime = option_msg.getStateTime();
            a.id = option_msg.getId();

            Option protoOption = this.behavior.options.get(a.id);
            a.option = new Option(protoOption); // make a shallow copy

            // TODO: parameters
            for (Messages.XABSLActionSparse sa : option_msg.getActiveSubActionsList()) {
                a.activeSubActions.add(parse(sa));
            }

            return a;
        } else if (action_msg.getType() == Messages.XABSLActionSparse.ActionType.SymbolAssignement) {
            XABSLAction.SymbolAssignement a = new XABSLAction.SymbolAssignement();
            
            SymbolKey key = new SymbolKey(parse(action_msg.getSymbol().getType()), 
                action_msg.getSymbol().getId());
                    
            a.symbol = parse(behavior.outputSymbols.get(key), action_msg.getSymbol());
            return a;
        } else if (action_msg.getType() == Messages.XABSLActionSparse.ActionType.BasicBehavior) {
            XABSLAction.BasicBehaviorExecution a = new XABSLAction.BasicBehaviorExecution();
            a.name = "Some Basic Behavior (not supported yet)";
            return a;
        }

        return null;
    }
    
    private SymbolType parse(Messages.XABSLSymbol.SymbolType type) {
        switch(type)
        {
            case Boolean: return BOOL;
            case Enum: return ENUM;
            case Decimal: return DECIMAL;
            default: return VOID;
        }
    }

    public XABSLBehavior parseComplete(Messages.BehaviorStateComplete behavior_msg) {
        this.behavior = new XABSLBehavior();

        // parse the enumerations first, so they are available when symbols are parsed
        for (int i = 0; i < behavior_msg.getEnumerationsCount(); i++) {
            behavior.enumerations.add(i, parse(behavior_msg.getEnumerations(i)));
        }
        for (int i = 0; i < behavior_msg.getOptionsCount(); i++) {
            behavior.options.add(i, parse(behavior_msg.getOptions(i)));
        }
        for (int i = 0; i < behavior_msg.getAgentsCount(); i++) {
            behavior.agents.add(i, parse(behavior_msg.getAgents(i)));
        }

        behavior.inputSymbols.putAll(parse(behavior_msg.getInputSymbolList()));
        behavior.outputSymbols.putAll(parse(behavior_msg.getOutputSymbolList()));

        return this.behavior;
    }

    public Map<SymbolKey, Symbol> parse(
            Messages.SymbolValueList valueList) {
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
            option.parameters.add(j, parse(option_msg.getParameters(j)));
        }

        for (int j = 0; j < option_msg.getStatesCount(); j++) {
            option.states.add(j, parse(option_msg.getStates(j)));
        }

        return option;
    }

    private Option.State parse(Messages.BehaviorStateComplete.Option.State state_msg) {
        return new Option.State(state_msg.getName(), state_msg.getTarget());
    }

    private Symbol parse(Messages.XABSLSymbol p_msg) 
    {
        Symbol p = new Symbol(p_msg.getName());
        return parse(p, p_msg);
    }

    private Symbol parse(Symbol p, Messages.XABSLSymbol p_msg) {
        switch (p_msg.getType()) {
            case Decimal:
                return p.createDecimal(p_msg.getDecimalValue());
            case Boolean:
                return p.createBoolean(p_msg.getBoolValue());
            case Enum:
                EnumType enumeration = this.behavior.enumerations.get(p_msg.getEnumTypeId());
                return p.createEnum(enumeration, p_msg.getEnumValue());
        }
        return p;
    }

    private Symbol parse(Symbol s, Messages.SymbolValueList.DoubleSymbol s_msg) {
        return s.createDecimal(s_msg.getValue());
    }

    private Symbol parse(Symbol s, Messages.SymbolValueList.BooleanSymbol s_msg) {
        return s.createBoolean(s_msg.getValue());
    }

    private Symbol parse(Symbol.Enum s, Messages.SymbolValueList.EnumSymbol s_msg) {
        return s.createEnum(s.enumType, s_msg.getValue());
    }

    private Agent parse(Messages.BehaviorStateComplete.Agent agent_msg) {
        Agent agent = new Agent();
        agent.name = agent_msg.getName();
        agent.rootOption = agent_msg.getRootOption();
        return agent;
    }

    private EnumType parse(Messages.BehaviorStateComplete.EnumType enum_msg) {
        EnumType enumeration = new EnumType(enum_msg.getName());
        for (Messages.BehaviorStateComplete.EnumType.Element msgElement : enum_msg.getElementsList()) {
            EnumType.Element e = new EnumType.Element(
                msgElement.getValue(),
                msgElement.getName().replace(enumeration.name+".", "")); // remove the preceeding type name, e.g., motion.type.walk => walk
            enumeration.elements.put(e.value, e);
        }
        return enumeration;
    }

    private EnumType.Element parse(Messages.BehaviorStateComplete.EnumType.Element element_msg) {
        EnumType.Element element = new EnumType.Element(
            (int) element_msg.getValue(),
            element_msg.getName());
        return element;
    }
}
