package de.hu_berlin.informatik.ki.nao.server;

import java.util.Map;

/**
 *
 * @author thomas
 */
public class Command
{

  private String name;
  private Map<String,byte[]> arguments;

  public Map<String, byte[]> getArguments()
  {
    return arguments;
  }

  public void setArguments(Map<String, byte[]> arguments)
  {
    this.arguments = arguments;
  }

  

  public String getName()
  {
    return name;
  }

  public void setName(String name)
  {
    this.name = name;
  }

  

}
