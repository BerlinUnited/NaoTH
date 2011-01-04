package de.hu_berlin.informatik.ki.nao.server;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.NotYetConnectedException;
import java.nio.channels.SocketChannel;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.commons.codec.binary.Base64;

/**
 * This class handles all debug and connection stuff.
 * Manager will register itself here and all communication is done through this
 * class.
 * @author thomas
 */
public class MessageServer
{

  /**
   * When using this lock you should have understood the meaning of the
   * following caller-graph.<br>
   * <b>Be warned an don't produce dead-locks!</b>
   * 
   * <pre>
   * 
   * +-------------+            
   * |MessageServer|            
   * +-------------+      +-----+
   *  |^                +-| GUI |
   *  ||                | +-----+
   *  ||                | | GUI |
   *  ||                | +-----+
   *  v|                |    ^   
   * +--------+ &lt;-------+    |   
   * |ManagerX| -------------+   
   * +--------+
   * 
   * </pre>
   */
  private final Lock LISTENER_LOCK = new ReentrantLock();
  public final static String STRING_ENCODING = "ISO-8859-15";
  private SocketChannel serverSocket;
  private Thread senderThread;
  private Thread receiverThread;
  private Thread periodicExecutionThread;
  private long updateIntervall = 60;
  private InetSocketAddress address;
  private List<CommandSender> listeners;
  private BlockingQueue<SingleExecEntry> commandRequestQueue;
  private BlockingQueue<SingleExecEntry> callbackQueue;
  private boolean isActive;
  private IMessageServerParent parent;
  private long receivedBytes;
  private long sentBytes;

  public MessageServer()
  {
    this(null);
  }

  public MessageServer(IMessageServerParent parent)
  {
    serverSocket = null;
    this.parent = parent;
    this.receivedBytes = 0;
    this.sentBytes = 0;

    listeners = new LinkedList<CommandSender>();

    commandRequestQueue = new LinkedBlockingQueue<SingleExecEntry>();
    callbackQueue = new LinkedBlockingQueue<SingleExecEntry>();
  }

  public void connect(String host, int port) throws IOException
  {
    if (serverSocket != null && serverSocket.isConnected())
    {
      isActive = false;
      serverSocket.close();
    }

    senderThread = new Thread(new Runnable()
    {

      public void run()
      {
        try
        {
          sendLoop();
        }
        catch (InterruptedException ex)
        {
          isActive = false;
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "thread was interupted", ex);
          disconnect();
          
        }
      }
    });

    receiverThread = new Thread(new Runnable()
    {

      public void run()
      {
        try
        {
          receiveLoop();
        }
        catch (InterruptedException ex)
        {
          isActive = false;
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "thread was interupted", ex);
          disconnect();
        }
      }
    });

    periodicExecutionThread = new Thread(new Runnable()
    {

      public void run()
      {
        periodicExecution();
      }
    });

    address = new InetSocketAddress(host, port);

    serverSocket = SocketChannel.open(address);
    while (!serverSocket.finishConnect())
    {
      // wait
    }

    serverSocket.configureBlocking(true);

    isActive = true;

    if (parent != null)
    {
      parent.showConnected(true);
    }

    periodicExecutionThread.start();
    senderThread.start();
    receiverThread.start();
  }//end connect

  public void disconnect()
  {
    isActive = false;
    if (serverSocket != null && serverSocket.isConnected())
    {
      for (SingleExecEntry entry : callbackQueue)
      {
        if (entry.sender != null)
        {
          entry.sender.handleError(-2);
        }
      }
      commandRequestQueue.clear();
      callbackQueue.clear();
      try
      {
        // disconnent
        serverSocket.close();
      }
      catch (IOException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
      }
      serverSocket = null;
      // notifiy disconnect
      if (parent != null)
      {
        parent.showConnected(false);
      } // end if
    }//end if
  }//end disconnect

  public InetSocketAddress getAddress()
  {
    return address;
  }//end getAddress

  /** Return whether RC is connected to a robot */
  public boolean isConnected()
  {
    return serverSocket != null && serverSocket.isConnected();
  }//end isConnected

  /**
   * Add a {@link CommandSender} to the repeating schedule.
   * A {@link CommandSender} will not be added twice.
   * @param commandSender
   */
  public void addCommandSender(CommandSender commandSender)
  {
    LISTENER_LOCK.lock();
    if (!listeners.contains(commandSender))
    {
      listeners.add(commandSender);
    }
    LISTENER_LOCK.unlock();
  }//end addCommandSender

  /**
   * Remove a {@link CommandSender} from the repeating schedule.
   * @param commandSender
   */
  public void removeCommandSender(CommandSender commandSender)
  {
    LISTENER_LOCK.lock();
    try
    {
      listeners.remove(commandSender);
    }
    finally
    {
      LISTENER_LOCK.unlock();
    }
  }//end removeCommandSender

  /**
   * Schedule a single command for execution. It is not guarantied when it
   * will be executed but the {@link CommandSender} will be notfied using the
   * right command as argument.
   * 
   * @param commandSender The command sender responsible for this command.
   * @param command       Instead of using {@link CommandSender#getCurrentCommand()}
   *                      this command is used.
   * 
   * @throws NotYetConnectedException is thrown if the server is not connected
   */
  public void executeSingleCommand(CommandSender commandSender, Command command)
    throws NotYetConnectedException
  {
    if (!isConnected())
    {
      throw new NotYetConnectedException();
    }

    SingleExecEntry e = new SingleExecEntry();
    e.command = command;
    e.sender = commandSender;
    try
    {
      commandRequestQueue.put(e);
    }
    catch (InterruptedException ex)
    {
      Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
    }


  }//end executeSingleCommand

  // send-receive-periodicExecution //
  public void receiveLoop() throws InterruptedException
  {
    while(isActive && serverSocket != null && serverSocket.isConnected())
    {
      try
      {
        StringBuilder sb = new StringBuilder();

        boolean valid = false;
        do
        {
          valid = false;

          ByteBuffer b = ByteBuffer.allocate(1);
          // reader answer
          serverSocket.read(b);
          receivedBytes++;
          
          byte c = b.get(0);
          if(c != 0)
          {
            valid = true;
          }
          else
          {
            boolean test = true;
          }

          sb.append((char) c);
        }
        while(valid);

        SingleExecEntry entry = callbackQueue.take();
        byte[] decoded = Base64.decodeBase64(sb.toString());

        if(entry.sender != null)
        {
          entry.sender.handleResponse(decoded, entry.command);
        }
      }
      catch (IOException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
        disconnect();
      }
    }
  }

  public void sendLoop() throws InterruptedException
  {
    while(isActive && serverSocket != null && serverSocket.isConnected())
    {
      SingleExecEntry entry = commandRequestQueue.take();
      callbackQueue.put(entry);

      Command c = entry.command;

      StringBuilder buffer = new StringBuilder();
      buffer.append("+").append(c.getName());
      if(c.getArguments() != null)
      {
        for(Map.Entry<String,byte[]> e : c.getArguments().entrySet())
        {
          boolean hasArg = e.getValue() != null;
          buffer.append(" ");
          if(hasArg)
          {
            buffer.append("+");
          }
          buffer.append(e.getKey());
          if(hasArg)
          {
            buffer.append(" ");
            buffer.append(new String(Base64.encodeBase64(e.getValue())));
          }
        }
      }
      buffer.append("\n");
      ByteBuffer bytes = ByteBuffer.wrap(buffer.toString().getBytes());
      try
      {
        sentBytes += serverSocket.write(bytes);
      }
      catch (IOException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
        disconnect();
      }
    }
  }

  public void periodicExecution()
  {
    while (isActive && serverSocket != null && serverSocket.isConnected())
    {
      try
      {
        try
        {
          long startTime = System.currentTimeMillis();

          sendPeriodicCommands();

          long stopTime = System.currentTimeMillis();
          long diff = updateIntervall - (stopTime - startTime);
          long wait = Math.max(0, diff);
          if (wait > 0)
          {
            Thread.sleep(wait);
          }
        }
        catch (InterruptedException ex)
        {
          isActive = false;
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "thread was interupted", ex);
          disconnect();
        }
      }
      catch (IOException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
      }
    } // while(isActive)
  }//end sendLoop

  public void sendPeriodicCommands() throws IOException
  {

    // periodic execution //

    // copy the listeners
    
    LinkedList<CommandSender> copyListener = new LinkedList<CommandSender>();
    LISTENER_LOCK.lock();
    try
    {
      copyListener.addAll(listeners);
    }
    finally
    {
      LISTENER_LOCK.unlock();
    }
    
    // check each command sender and perform a request
    for (CommandSender sender : copyListener)
    {
      try
      {
        // send command and get generated ID
        SingleExecEntry e = new SingleExecEntry();
        e.command = sender.getCurrentCommand();
        e.sender = sender;

        commandRequestQueue.put(e);
      }
      catch (InterruptedException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
          "interrupted in periodic command execution, will disconnect", ex);
        disconnect();
      }
    }  // end for each listenerF
  }//end sendPeriodicCommands
 
  public ByteBuffer byteBufferFromInt(int value)
  {
    ByteBuffer b = ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN);
    b.putInt(value);
    return b;
  }//end byteBufferFromInt

  private class SingleExecEntry
  {

    public CommandSender sender;
    public Command command;
  }//end SingleExecEntry

  public List<CommandSender> getListeners()
  {
    return listeners;
  }//end getListeners

  public long getReceivedBytes()
  {
    return receivedBytes;
  }//end getReceivedBytes

  public long getSentBytes()
  {
    return sentBytes;
  }//end getSentBytes
}//end class MessageServer

