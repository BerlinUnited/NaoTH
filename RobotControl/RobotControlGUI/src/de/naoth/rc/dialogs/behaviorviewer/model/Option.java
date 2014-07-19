/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.dialogs.behaviorviewer.model;

import java.util.ArrayList;

/**
 *
 * @author thomas
 */
public class Option {

    public static class State {

        public State(String name, boolean target) {
            this.name = name;
            this.target = target;
        }
        public final String name;
        public final boolean target;
    }

    public Option(String name) {
        this.name = name;
    }

    public Option(Option other) {
        name = other.name;
        states.addAll(other.states);
        parameters.addAll(other.parameters);
    }
    public final String name;
    public final ArrayList<State> states = new ArrayList<State>();
    public final ArrayList<Symbol> parameters = new ArrayList<Symbol>();
    
}
