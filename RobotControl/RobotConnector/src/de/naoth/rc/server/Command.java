package de.naoth.rc.server;

import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author thomas
 */
public class Command
{

  private String name;
  private Map<String,byte[]> arguments;

  public Command()
  {
    this("");
  }

  /**
   * Constructs a new command with a name and no arguments
   * @param name
   */
  public Command(String name)
  {
    this.name = name;
  }

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

  /**
   * Add a new argument, will replace an old one if an argument with the same
   * name already exists.
   * @param name
   * @param value Value as byte array
   * @return Will return itself.
   */
  public Command addArg(String name, byte[] value)
  {
    if(arguments == null)
    {
      arguments = new HashMap<String, byte[]>();
    }
    
    arguments.put(name, value);

    return this;
  }
  
  /**
   * Add a new argument with a String value.
   * @param name
   * @param value The value as string, will be converted to a byte array
   * @return Will return itself
   * 
   * @see #addArg(java.lang.String, byte[]) 
   */
  public Command addArg(String name, String value)
  {
    if(value == null)
    {
      addArg(name);
    }
    else
    {
      addArg(name, value.getBytes());
    }
    return this;
  }

  /**
   * Adds an arguments with a name and no value
   * @param name
   * @return Will return itself.
   */
  public Command addArg(String name)
  {
    if(arguments == null)
    {
      arguments = new HashMap<String, byte[]>();
    }
    arguments.put(name, null);
    return this;
  }

  @Override
  public String toString() {
      return this.name;
  }
}
