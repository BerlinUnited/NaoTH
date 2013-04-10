package de.naoth.naoscp;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */


import javax.jmdns.ServiceInfo;
import java.net.*;

/**
 *
 * @author thomas
 */
public class NaoSshWrapper
{
  private ServiceInfo info;
  private InetAddress[] addresses;
  private String host;
  private String type;
  
  public NaoSshWrapper(ServiceInfo info, String type)
  {
    this.info = info;
    this.type = type;
    updateHost();
  }

  public NaoSshWrapper(ServiceInfo info)
  {
    this.info = info;
    this.type = "";
    updateHost();
  }
  
  private void updateHost()
  {
//    host = info.getHostAddress();
    host = "";
    if(info.getHostAddresses().length > 0)
    {
      try
      {
        addresses = InetAddress.getAllByName(info.getInetAddresses()[0].getCanonicalHostName());
        host = addresses[0].getHostAddress();
      }
      catch(UnknownHostException ex)
      {
        addresses = new InetAddress[0];
        host = info.getHostAddresses()[0];
      }
    }
  }
  
  public ServiceInfo getInfo()
  {
    return info;
  }

  public String getType()
  {
    return type;
  }

  public void setInfo(ServiceInfo info)
  {
    this.info = info;
    updateHost();
  }
  
  public boolean isValid()
  {
    return host.length() > 0;
  }

  public String getDefaultHostAddress()
  {
    return host;
  }

  public String[] getHostAddresses()
  {
    return info.getHostAddresses();
  }
  
  public InetAddress[] getAddresses()
  {
    return addresses;
  }
    
  @Override
  public String toString()
  {
    return type + ": " + info.getName() +  " (" + host + ")";
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
    final NaoSshWrapper other = (NaoSshWrapper) obj;
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
