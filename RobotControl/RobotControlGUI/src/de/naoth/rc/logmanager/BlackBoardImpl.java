/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.logmanager;

import java.util.HashMap;
import java.util.LinkedHashSet;
import java.util.Map;
import java.util.Set;

/**
 *
 * @author thomas
 */
public class BlackBoardImpl implements BlackBoard {
    private final Map<String,LogDataFrame> registry = new HashMap<>();
    
    public void add(LogDataFrame f) {
        registry.put(f.getName(), f);
    }
    
    @Override
    public boolean has(String name) {
        return registry.containsKey(name);
    }
    
    @Override
    public LogDataFrame get(String name) {
        return registry.get(name);
    }

  @Override
  public Set<String> getNames()
  {
    return new LinkedHashSet<>(registry.keySet());
  }
    
    
}
