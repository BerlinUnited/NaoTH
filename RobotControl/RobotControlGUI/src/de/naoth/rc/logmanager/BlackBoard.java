/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.logmanager;

import java.util.Set;

/**
 *
 * @author thomas
 */
public interface BlackBoard
{
  public Set<String> getNames();
  public LogDataFrame get(String name);

}
