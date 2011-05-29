/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.dataformats;

import java.util.ArrayList;

/**
 *
 * @author thomas
 */
public class ModuleConfiguration
{

  private ArrayList<Node> nodeList;

  public ModuleConfiguration()
  {
    nodeList = new ArrayList<Node>();
  }

  public void addVertex(Node vertex)
  {
    // collect nodes to nodeList to preserve the order
    nodeList.add(vertex);
  }//end addVertex

  public ArrayList<Node> getNodeList() {
    return nodeList;
  }
  
  public static enum NodeType
  {
    Module,
    Represenation
  };

  public static class Node
  {
    private String name;
    private NodeType type;
    private boolean enabled;

    public Node(String name, NodeType type, boolean enabled)
    {
      this.name = name;
      this.type = type;
      this.enabled = enabled;
    }

    public String getName()
    {
      return name;
    }

    public NodeType getType()
    {
      return type;
    }

    public boolean isEnabled()
    {
      return enabled;
    }

    @Override
    public boolean equals(Object obj)
    {
      if(obj == null)
      {
        return false;
      }
      if(getClass() != obj.getClass())
      {
        return false;
      }
      final Node other = (Node) obj;
      if((this.name == null) ? (other.name != null) : !this.name.equals(other.name))
      {
        return false;
      }
      if(this.type != other.type)
      {
        return false;
      }
      return true;
    }

    @Override
    public int hashCode()
    {
      int hash = 5;
      hash = 47 * hash + (this.name != null ? this.name.hashCode() : 0);
      hash = 47 * hash + (this.type != null ? this.type.hashCode() : 0);
      return hash;
    }
  }//end class Node

  public static class Edge
  {
    
  }

}//end class ModuleConfiguration
