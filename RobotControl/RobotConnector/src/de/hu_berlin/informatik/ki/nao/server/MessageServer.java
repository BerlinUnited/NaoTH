package de.hu_berlin.informatik.ki.nao.server;

import com.google.protobuf.ByteString;
import de.hu_berlin.informatik.ki.nao.messages.Messages.CMD;
import de.hu_berlin.informatik.ki.nao.messages.Messages.CMDArg;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.NotYetConnectedException;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
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
  private InetSocketAddress address;
  private Socket serverSocket;
  private Thread senderThread;
  private Thread receiverThread;
  private Thread periodicExecutionThread;
  private List<CommandSender> listeners;
  private BlockingQueue<SingleExecEntry> commandRequestQueue;
  private BlockingQueue<SingleExecEntry> callbackQueue;
  private IMessageServerParent parent;
  private long receivedBytes;
  private long sentBytes;
  
  private AtomicInteger pendingFrames = new AtomicInteger();
  private long updateIntervall = 33;
  // dpends on the assumed maximum network delay
  private long maximalBufferedFrames = 3;
  
  private Base64 base64 = new Base64();
  

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

    this.listeners = new LinkedList<CommandSender>();

    this.commandRequestQueue = new LinkedBlockingQueue<SingleExecEntry>();
    this.callbackQueue = new LinkedBlockingQueue<SingleExecEntry>();
    
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
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "thread was interupted", ex);
        }
      }
    });
    
  }

  public void connect(String host, int port) throws IOException
  {
    if (serverSocket != null && serverSocket.isConnected())
    {
      serverSocket.close();
    }

    // define the threads
    
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
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "thread was interupted", ex);
        }
        catch(SocketException ex)
        {
          // ignore
        }
        catch (IOException ex)
        {
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "socket read failed", ex);
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
    serverSocket = new Socket();
    serverSocket.connect(address, 1000);

    if (parent != null)
    {
      parent.showConnected(true);
    }

    // clean all old stuff in the pipe
    while (!serverSocket.isClosed() && serverSocket.getInputStream().available() > 0)
    {
      serverSocket.getInputStream().read();
      // nothing
    }

    // start threads
    
    if(!senderThread.isAlive())
    {
      // start sender only once
      senderThread.start();
    }
    
    periodicExecutionThread.start();
    receiverThread.start();
  }//end connect

  public void disconnect()
  {    
    if(!isConnected())
    {
      // nothing to do
      return;
    }
    
    try
    {
      // disconnect
      serverSocket.close();
      serverSocket = null;
            
      // wait until no new commands are inserted
      periodicExecutionThread.join();
      
      // clear commands in order to shutdown
      commandRequestQueue.clear();
      
      // wait until rest of the thread are settled
      receiverThread.join();
      
      pendingFrames.set(0);
      
      // clear queues and send remaining error messages
      for (SingleExecEntry entry : callbackQueue)
      {
        if (entry.sender != null)
        {
          entry.sender.handleError(-2);
        }
      }
      callbackQueue.clear();
      
    }
    catch (InterruptedException ex)
    {
      Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
    }    
    catch (IOException ex)
    {
      Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
    }
    finally
    {
      serverSocket = null;
      // notifiy disconnect
      if (parent != null)
      {
        parent.showConnected(false);
      } // end if
    }
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
    try
    {
      if (!listeners.contains(commandSender))
      {
        listeners.add(commandSender);
      }
    }
    finally
    {
      LISTENER_LOCK.unlock();
    }
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
  public void receiveLoop() throws InterruptedException, IOException
  {
    while (isConnected())
    {
      ByteBuffer header = ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN);
      
      // reader header
      int headerRec = serverSocket.getInputStream().read(header.array());
      if(headerRec == 4)
      {
        receivedBytes += headerRec;
        int msgSize = header.getInt();
        
        ByteBuffer content = ByteBuffer.allocate(msgSize);
        int pos=0;
        while(pos < msgSize)
        {
          int readBytes = serverSocket.getInputStream()
            .read(content.array(), pos, msgSize - pos);
          pos += readBytes;
          receivedBytes += readBytes;
        }
        decodeAndHandleMessage(content.array());
      }
            
      try
      {
        Thread.sleep(1);
      }
      catch(InterruptedException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
      }
    }//end while
  }//end receiveLoop

  public void sendLoop() throws InterruptedException
  {
    while (true)
    {
      SingleExecEntry entry = commandRequestQueue.poll(1, TimeUnit.SECONDS);
      if(entry == null)
      {
        // fire an empty message to check the connection
        try
        {
          if(isConnected())
          {
            ByteBuffer header = ByteBuffer.allocate(4);            
            header.putInt(Integer.reverseBytes(0));
            
            serverSocket.getOutputStream().write(header.array());
          }
        }
        catch (SocketException ex)
        {
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
          disconnect();
        }
        catch (IOException ex)
        {
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
          disconnect();
        }
      }
      else
      {
        callbackQueue.put(entry);

        Command c = entry.command;
        CMD.Builder cmdBuilder = CMD.newBuilder();
        
        cmdBuilder = cmdBuilder.setName(c.getName());
        
        if (c.getArguments() != null)
        {
          for (Map.Entry<String, byte[]> e : c.getArguments().entrySet())
          {
            CMDArg.Builder arg = CMDArg.newBuilder().setName(e.getKey());
            if(e.getValue() != null)
            {
              arg = arg.setBytes(ByteString.copyFrom(e.getValue()));
            }
            cmdBuilder = cmdBuilder.addArgs(arg);
          }
        }
        try
        {
          if(isConnected())
          {
            CMD cmd = cmdBuilder.build();
            byte[] bytes = cmd.toByteArray();
            
            // write first header
            ByteBuffer header = ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN);
            
            header.putInt(bytes.length);
            serverSocket.getOutputStream().write(header.array());
            serverSocket.getOutputStream().write(bytes);
            sentBytes += bytes.length;
          }
        }
        catch (IOException ex)
        {
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
          disconnect();
        }
      }
    }
  }
  
  private void decodeAndHandleMessage(byte[] bytes) throws InterruptedException
  {
    SingleExecEntry entry = callbackQueue.take();
    byte[] decoded = base64.decode(bytes);

    if(entry != null)
    {
      if(entry.command != null && "endFrame".equals(entry.command.getName()))
      {
        pendingFrames.decrementAndGet();
      }
      else if(entry.sender != null)
      {        
        entry.sender.handleResponse(decoded, entry.command);
      }
    }
  }//end decodeAndHandleMessage


  public void periodicExecution()
  {
    while (isConnected())
    {
      try
      {
        try
        {

          long startTime = System.currentTimeMillis();
          
          // do not send if the robot is still busy
          while(isConnected() && pendingFrames.get() >= maximalBufferedFrames)
          {
            Thread.sleep(10);
          }
          
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
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "thread was interupted", ex);
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
          "interrupted in periodic command execution", ex);
      }
    }  // end for each listener
    
    SingleExecEntry eFrameEnd = new SingleExecEntry();
    eFrameEnd.command = new Command("endFrame");
    try
    {
      commandRequestQueue.put(eFrameEnd);
      pendingFrames.incrementAndGet();
    }
    catch (InterruptedException ex)
    {
      Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
    }
    
  }//end sendPeriodicCommands

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


  
  public class CommandSenderWithByteCount implements CommandSender
  {
    CommandSender commandSender;
    
    public CommandSenderWithByteCount(CommandSender commandSender)
    {
      this.commandSender = commandSender;
    }

    public Command getCurrentCommand() {
      return this.commandSender.getCurrentCommand();
    }

    public void handleError(int code) {
      this.commandSender.handleError(code);
    }

    public void handleResponse(byte[] result, Command originalCommand) {
      this.commandSender.handleResponse(result, originalCommand);
    }
  }//end class CommandSenderWithByteCount
}//end class MessageServer

