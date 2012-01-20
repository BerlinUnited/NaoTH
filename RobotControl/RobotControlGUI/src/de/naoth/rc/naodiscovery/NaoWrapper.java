/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.naodiscovery;

import javax.jmdns.ServiceInfo;

/**
 *
 * @author thomas
 */
public class NaoWrapper
{
  private ServiceInfo info;
  private String host;
  
  public NaoWrapper(ServiceInfo info)
  {
    this.info = info;
    updateHost();
  }

  private void updateHost()
  {
    if(info.getHostAddresses().length > 0)
    {
      host = info.getHostAddresses()[0];
    }
    else
    {
      host = "unknown robot";
    }
  }
  
  public ServiceInfo getInfo()
  {
    return info;
  }

  public void setInfo(ServiceInfo info)
  {
    this.info = info;
    updateHost();
  }
  
  public boolean isValid()
  {
    return info.getHostAddresses().length > 0;
  }

  @Override
  public String toString()
  {
    return info.getName() +  " (" + host + ")";
  }

  @Override
  public boolean equals(Object obj)
  {
    if (obj == null)
    {
      return false;
    }
    if (getClass() != obj.getClass())
    {
      return false;
    }
    final NaoWrapper other = (NaoWrapper) obj;
    if ((this.host == null) ? (other.host != null)
      : !this.host.equals(other.host))
    {
      return false;
    }
    return true;
  }

  @Override
  public int hashCode()
  {
    int hash = 3;
    hash = 17 * hash + (this.host != null ? this.host.hashCode() : 0);
    return hash;
  }
  
  
  
  
}
