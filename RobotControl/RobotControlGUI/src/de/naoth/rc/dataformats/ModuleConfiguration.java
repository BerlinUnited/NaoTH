/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.dataformats;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author thomas
 */
public class ModuleConfiguration
{

  private final Map<String,Node> nodeMap;
  private final ArrayList<Node> nodeList;

  public ModuleConfiguration()
  {
    nodeList = new ArrayList<Node>();
    nodeMap = new HashMap<String,Node>();
  }
  
  public void addEdge(Node vertexOne, Node vertexTwo)
  {
      vertexOne.provide.add(vertexTwo);
      vertexTwo.require.add(vertexOne);
  }//end addVertex

  public Node addNode(String name, String path, NodeType type, boolean enabled)
  {
    Node n = nodeMap.get(name);
    if(n == null) {
        n = new Node(name,path,type,enabled);
        nodeMap.put(name, n);
        nodeList.add(n);
    }
    return n;
  }//end addVertex
  
  private void addVertex(Node vertex)
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
    private String path;
    private NodeType type;
    private boolean enabled;
    public ArrayList<Node> require = new ArrayList<Node>();
    public ArrayList<Node> provide = new ArrayList<Node>();

    public Node(String name, String path, NodeType type, boolean enabled)
    {
      this.name = name;
      this.path = path;
      this.type = type;
      this.enabled = enabled;
    }

    public String getName()
    {
      return name;
    }
    
    public String getPath()
    {
      return path;
    }

    public NodeType getType()
    {
      return type;
    }

    public boolean isEnabled()
    {
      return enabled;
    }
    
    /**
     * Returns a cleaned up version of the path, without any 
     * ".." and with the prepended process name
     * @param processName
     * @return 
     */
    public String getCleanPathForProcess(String processName)
    {
      String result = path;
      int k = result.toLowerCase().indexOf(processName.toLowerCase());
      if (k == -1)
      {
        int i = result.lastIndexOf(':');
        result = processName.toLowerCase() + result.substring(i);
      }
      else
      {
        result = processName + result.substring(k + processName.length());
      }
      return result;
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
      if(this.type != other.type)
      {
        return false;
      }
      if((this.name == null) ? (other.name != null) : !this.name.equals(other.name))
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
    
    @Override
    public String toString() {
        return this.getName();
    }
  }
  public static class Edge
  {
    
  }

}//end class ModuleConfiguration
