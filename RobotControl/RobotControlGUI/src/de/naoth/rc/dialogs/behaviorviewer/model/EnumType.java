/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.dialogs.behaviorviewer.model;

import java.util.Map;
import java.util.TreeMap;

/**
 *
 * @author thomas
 */
public class EnumType {

    public static class Element {

        public String name;
        public int value;
    }
    public String name;
    public Map<Integer, Element> elements = new TreeMap<>();
    
}
