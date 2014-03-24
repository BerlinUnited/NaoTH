/*
 * 
 */
package de.naoth.rc.dialogs.behaviorviewer;

import de.naoth.rc.messages.Messages;

/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLProtoParser {

    private XABSLBehavior behavior = null;

    
    public XABSLBehaviorFrame parse(Messages.BehaviorStatusSparse status)
    {
        XABSLBehaviorFrame frame = new XABSLBehaviorFrame();
        
        for (Messages.XABSLSymbol symbol : status.getInputSymbolsList()) {
            XABSLBehavior.Symbol s = behavior.inputSymbols.get(symbol.getId());
            frame.inputSymbols.add(parse(s,symbol));
        }

        for (Messages.XABSLSymbol symbol : status.getOutputSymbolsList()) {
            XABSLBehavior.Symbol s = behavior.outputSymbols.get(symbol.getId());
            frame.outputSymbols.add(parse(s,symbol));
        }

        for (Messages.XABSLActionSparse a : status.getActiveRootActionsList()) {
            frame.actions.add(parse(a));
        }
        return frame;
    }
    
            
    public XABSLAction parse(Messages.XABSLActionSparse action_msg)
    {
        if(action_msg.getType() == Messages.XABSLActionSparse.ActionType.Option) {
            Messages.XABSLActiveOptionSparse option_msg = action_msg.getOption();
            XABSLAction.OptionExecution a = new XABSLAction.OptionExecution();
            
            a.activeState = option_msg.getActiveState();
            a.timeOfExecution = option_msg.getTimeOfExecution();
            a.stateTime = option_msg.getStateTime();
            a.id = option_msg.getId();
            
            XABSLBehavior.Option protoOption = this.behavior.options.get(a.id);
            a.option = new XABSLBehavior.Option(protoOption); // make a shallow copy

            // TODO: parameters
            
            for(Messages.XABSLActionSparse sa: option_msg.getActiveSubActionsList()) {
                a.activeSubActions.add(parse(sa));
            }
            
            return a;
        } else if(action_msg.getType() == Messages.XABSLActionSparse.ActionType.SymbolAssignement) {
            XABSLAction.SymbolAssignement a = new XABSLAction.SymbolAssignement();
            a.symbol = parse(behavior.outputSymbols.get(action_msg.getSymbol().getId()),action_msg.getSymbol());
            return a;
        } else if(action_msg.getType() == Messages.XABSLActionSparse.ActionType.BasicBehavior) {
            XABSLAction.BasicBehaviorExecution a = new XABSLAction.BasicBehaviorExecution();
            a.name = "Some Basic Behavior (not supported yet)";
            return a;
        }
        
        return null;
    }
    
    
    public XABSLBehavior parse(Messages.XABSLBehavior behavior_msg) {
        this.behavior = new XABSLBehavior();

        // parse the enumerations first, so they are avaliable when symbols are parsed
        for (int i = 0; i < behavior_msg.getEnumerationsCount(); i++) {
            behavior.enumerations.add(i, parse(behavior_msg.getEnumerations(i)));
        }
        for (int i = 0; i < behavior_msg.getOptionsCount(); i++) {
            behavior.options.add(i, parse(behavior_msg.getOptions(i)));
        }
        for (int i = 0; i < behavior_msg.getAgentsCount(); i++) {
            behavior.agents.add(i, parse(behavior_msg.getAgents(i)));
        }
        for (int i = 0; i < behavior_msg.getInputSymbolsCount(); i++) {
            behavior.inputSymbols.add(i, parse(behavior_msg.getInputSymbols(i)));
        }
        for (int i = 0; i < behavior_msg.getOutputSymbolsCount(); i++) {
            behavior.outputSymbols.add(i, parse(behavior_msg.getOutputSymbols(i)));
        }

        return this.behavior;
    }

    private XABSLBehavior.Option parse(Messages.XABSLBehavior.Option option_msg) {
        XABSLBehavior.Option option = new XABSLBehavior.Option(option_msg.getName());
        
        for (int j = 0; j < option_msg.getParametersCount(); j++) {
            option.parameters.add(j, parse(option_msg.getParameters(j)));
        }

        for (int j = 0; j < option_msg.getStatesCount(); j++) {
            option.states.add(j, parse(option_msg.getStates(j)));
        }

        return option;
    }

    private XABSLBehavior.Option.State parse(Messages.XABSLBehavior.Option.State state_msg) {
        return new XABSLBehavior.Option.State(state_msg.getName(), state_msg.getTarget());
    }

    private XABSLBehavior.Symbol parse(Messages.XABSLSymbol p_msg) {
        XABSLBehavior.Symbol p = new XABSLBehavior.Symbol(p_msg.getName());
        return parse(p, p_msg);
    }
    
    private XABSLBehavior.Symbol parse(XABSLBehavior.Symbol p, Messages.XABSLSymbol p_msg) {
        switch (p_msg.getType()) {
            case Decimal:
                return p.createDecimal(p_msg.getDecimalValue());
            case Boolean:
                return p.createBoolean(p_msg.getBoolValue());
            case Enum:
                XABSLBehavior.EnumType enumeration = this.behavior.enumerations.get(p_msg.getEnumTypeId());
                return p.createEnum(enumeration, p_msg.getEnumValue());
        }
        return p;
    }

    private XABSLBehavior.Agent parse(Messages.XABSLBehavior.Agent agent_msg) {
        XABSLBehavior.Agent agent = new XABSLBehavior.Agent();
        agent.name = agent_msg.getName();
        agent.rootOption = agent_msg.getRootOption();
        return agent;
    }

    private XABSLBehavior.EnumType parse(Messages.XABSLBehavior.EnumType enum_msg) {
        XABSLBehavior.EnumType enumeration = new XABSLBehavior.EnumType();
        enumeration.name = enum_msg.getName();
        for (int i = 0; i < enum_msg.getElementsCount(); i++) {
            enumeration.elements.add(i, parse(enum_msg.getElements(i)));
        }
        return enumeration;
    }

    private XABSLBehavior.EnumType.Element parse(Messages.XABSLBehavior.EnumType.Element element_msg) {
        XABSLBehavior.EnumType.Element element = new XABSLBehavior.EnumType.Element();
        element.name = element_msg.getName();
        element.value = (int) element_msg.getValue();
        return element;
    }
}
