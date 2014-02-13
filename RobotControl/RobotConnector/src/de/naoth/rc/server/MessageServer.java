package de.naoth.rc.server;

import com.google.protobuf.ByteString;
import de.naoth.rc.messages.Messages.CMD;
import de.naoth.rc.messages.Messages.CMDArg;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.NotYetConnectedException;
import java.nio.channels.SocketChannel;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class handles all debug and connection stuff.
 * Manager will register itself here and all communication is done through this
 * class.
 * @author thomas
 */
public class MessageServer extends AbstractMessageServer
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
  private final Lock SINGLE_EXE_LOCK = new ReentrantLock();
  
  public final static String STRING_ENCODING = "ISO-8859-15";
  private final long updateIntervall = 33;
  
  private SocketChannel serverSocket;
  private Thread senderThread;
  private InetSocketAddress address;
  private List<CommandSender> listeners;
  private List<SingleExecEntry> singleExec;
  private List<SingleExecEntry> answerRequestQueue;
  private boolean isActive;

  private long receivedBytes;
  private long sentBytes;

  public MessageServer()
  {
    this.serverSocket = null;
    this.receivedBytes = 0;
    this.sentBytes = 0;

    this.listeners = new LinkedList<CommandSender>();
    this.singleExec = new LinkedList<SingleExecEntry>();
    this.answerRequestQueue = Collections.synchronizedList(new LinkedList<SingleExecEntry>());
  }

  public void connect(String host, int port) throws IOException
  {
    // close previous connection if necessary
    disconnect();
    
    address = new InetSocketAddress(host, port);

    // open and configure the socket
    serverSocket = SocketChannel.open();

    //serverSocket.socket().setSoTimeout(1000);
    serverSocket.configureBlocking(false);
    serverSocket.connect(address);

    // wait a little...
    int trials = 0;
    while(!serverSocket.finishConnect())
    {
      trials++;
      try{
        Thread.sleep(100);
      }catch(InterruptedException e){}

      if(trials > 10)
        throw new IOException("Couldn't connect to address " + address );
    }//end while

    serverSocket.configureBlocking(true);

    
    isActive = true;

    this.fireConnected(this.address);

    // create and start the sender thread
    this.senderThread = new Thread(new Runnable()
    {
      @Override
      public void run()
      {
        try
        {
            sendReceiveLoop();
        }
        catch(InterruptedException ex)
        {
          isActive = false;
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "thread was interupted", ex);
          close_connection();
          MessageServer.this.fireDisconnected(ex.getMessage());
        }
        catch(Exception ex)
        {
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
            "Unexpected exception...",
            ex);
          close_connection();
          MessageServer.this.fireDisconnected(ex.getMessage());
        }
      }
    });
    
    senderThread.start();
  }//end connect

  
  public void close_connection()
  {
    if(serverSocket != null && serverSocket.isConnected())
    {
      // call error handlers of remaining requests
      for(SingleExecEntry a : answerRequestQueue)
      {
        if(a.sender != null)
        {
          a.sender.handleError(-2);
        }
      }
      answerRequestQueue.clear();

      // disconnect
      try
      {
        serverSocket.close();
      }
      catch(IOException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
            "Unexpected exception...",
            ex);
      }
      serverSocket = null;
    }//end if
  }//end close_connection
  
  
  public void disconnect()
  {
    isActive = false;
    
    // wait until the sender Thread is dead
    
    if(senderThread != null)
    {
      try {
        senderThread.join(1000); // wait max one second for the thread to stop
      } catch (InterruptedException e) { /* ignore */ }
    }//end if
    
    close_connection();
    MessageServer.this.fireDisconnected(null);
    
  }//end disconnect

  
  public InetSocketAddress getAddress()
  {
    return address;
  }

  
  /** Return whether RC is connected to a robot */
  public boolean isConnected()
  {
    return serverSocket != null && serverSocket.isConnected();
  }

  /**
   * Add a {@link CommandSender} to the repeating schedule.
   * A {@link CommandSender} will not be added twice.
   * @param commandSender
   */
  public void addCommandSender(CommandSender commandSender)
  {
    LISTENER_LOCK.lock();
    if(!listeners.contains(commandSender))
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
    listeners.remove(commandSender);
    LISTENER_LOCK.unlock();
  }//end removeCommandSender

  /**
   * Schedule a single command for execution. It is not guaranteed when it
   * will be executed but the {@link CommandSender} will be notified using the
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
    if(!isConnected())
      throw new NotYetConnectedException();

    SINGLE_EXE_LOCK.lock();
    SingleExecEntry e = new SingleExecEntry();
    e.command = command;
    e.sender = commandSender;
    singleExec.add(e);
    SINGLE_EXE_LOCK.unlock();
  }//end executeSingleCommand

  /**
   * Schedule a single command for execution. It is not guaranteed when it
   * will be executed but the {@link CommandSender} will be notified using the
   * right command as argument.
   * 
   * @param commandSender The command sender responsible for this command.
   * 
   * @throws NotYetConnectedException is thrown if the server is not connected
   */
  public void executeSingleCommand(CommandSender commandSender)
          throws NotYetConnectedException
  {
    executeSingleCommand(commandSender, commandSender.getCurrentCommand());
  }
  
  
  // send-receive-loop //
  public void sendReceiveLoop() throws Exception, InterruptedException
  {
      while(isActive && serverSocket != null && serverSocket.isConnected())
      {
          long startTime = System.currentTimeMillis();

          pollAnswers();
          sendPendingCommands();
          while(answerRequestQueue.size() > 0)
          {
            pollAnswers();
            // HACK: prevent deadlock on single core machines
            // ...for Thomas to fix :)
            Thread.sleep(5);
          }
          long stopTime = System.currentTimeMillis();
          long diff = updateIntervall - (stopTime-startTime);
          long wait = Math.max(0, diff);
          if(wait > 0)
          {
            Thread.sleep(wait);
          }
      } // while(isActive)
  }//end sendReceiveLoop

      
      
  public void sendPendingCommands() throws IOException
  {
    // single execution //

    serverSocket.configureBlocking(true);

    // copy
    SINGLE_EXE_LOCK.lock();
    LinkedList<SingleExecEntry> copySingle = new LinkedList<SingleExecEntry>();
    copySingle.addAll(singleExec);
    singleExec.clear();
    SINGLE_EXE_LOCK.unlock();

    for(SingleExecEntry e : copySingle)
    {
      try
      {
        sendSingleCommandReturn(serverSocket, e.command);

        answerRequestQueue.add(e);
      }
      catch(IOException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
          "writing to the network stream to the robot failed, " +
          "will disconnect", ex);
        disconnect();
      }
    }//end for

    // periodic execution //

    // copy the listeners
    LISTENER_LOCK.lock();
    LinkedList<CommandSender> copyListener = new LinkedList<CommandSender>();
    copyListener.addAll(listeners);
    LISTENER_LOCK.unlock();

    // check each command sender and perform a request
    for(CommandSender sender : copyListener)
    {
      try
      {
        // clear buffer, just to be sure...

        // send command and get generated ID
        Command command = sender.getCurrentCommand();

        
        sendSingleCommandReturn(serverSocket, command);

        SingleExecEntry e = new SingleExecEntry();
        e.command = command;
        e.sender = sender;
        answerRequestQueue.add(e);

      }
      catch(IOException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "writing to the network stream to the robot failed, " +
          "will disconnect", ex);
        disconnect();
      }
    }  // end for each listenerF
  }//end sendPendingCommands

  public void pollAnswers() throws IOException
  {

    boolean somethingFound = true;
    while(somethingFound)
    {
      somethingFound = false;
      
      serverSocket.configureBlocking(false);
      // read size of data
      
      ByteBuffer sizeBuffer = readContent(serverSocket, 4);
      
      if(sizeBuffer == null)
      {
        return;
      }
      somethingFound = true;
      
      int size = sizeBuffer.getInt(0);

      serverSocket.configureBlocking(true);
      
      // approximated size of received data
      receivedBytes += size;

      SingleExecEntry e = answerRequestQueue.remove(0);

      try
      {
        if(e.sender != null)
        {
          ByteBuffer data = readContent(serverSocket, size);

          if(data == null)
            throw new IOException("No data could be read from the socket.");

          // call sender
          e.sender.handleResponse(data.array(), e.command);
        }
      }
      catch(IOException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
          "reading from the network stream from the robot failed, will disconnect",
          ex);
        disconnect();
      }
    }//end while
  }//end pollAnswers


  // helpers
  private void sendSingleCommandReturn(SocketChannel socket, Command command) throws IOException
  {
    // convert to Protobuf
    CMD.Builder cmd = CMD.newBuilder().setName(command.getName());
    
    if(command.getArguments() != null)
    {
      for(Map.Entry<String, byte[]> e : command.getArguments().entrySet())
      {
        CMDArg.Builder arg = CMDArg.newBuilder()
          .setName(e.getKey());
        if(e.getValue() != null)
        {
          arg = arg.setBytes(ByteString.copyFrom(e.getValue()));
        }
        cmd = cmd.addArgs(arg);
      }
    }
    
    byte[] commandAsByteArray = cmd.build().toByteArray();
    int intLength = commandAsByteArray.length;
    
    ByteBuffer bLength = byteBufferFromInt(intLength);
   
    ByteBuffer bCommand = ByteBuffer.wrap(commandAsByteArray);

    // write length    
    sentBytes += socket.write(bLength);
    // write data
    sentBytes += socket.write(bCommand);

  }//end sendSingleCommandReturnID

  
  private ByteBuffer readContent(SocketChannel socket, int length) throws IOException
  {
    if(length < 0)
    {
      //System.err.println("invalid message length: " + length);
      //return null;
      throw new IOException("invalid message length: " + length);
    }
    else if(length > 10485760) // more than 10 MB
    {
      //System.err.println("Message size received bigger than 10 MB, this is too big! Will disconnect.");
      //disconnect();
      //return null;
      throw new IOException("Message size received bigger than 10 MB, this is too big! Will disconnect.");
    }

    if(socket == null)
    {
      throw new IOException("Socket is null.");
    }
    
    ByteBuffer buffer = ByteBuffer.allocate(length).order(ByteOrder.LITTLE_ENDIAN);

    int byteCountTotal = 0;
    int byteCountSingle = 0;
    while((byteCountSingle = socket.read(buffer)) > 0)
    {
      byteCountTotal += byteCountSingle;
    }
    
    if(byteCountTotal == length)
    {
      buffer.flip();
      return buffer;
    }
    else if(byteCountSingle == -1)
    {
      throw new IOException("socket stream closed");
    }
    else
    {
      return null;
    }
  }//end readContent

  
  public ByteBuffer byteBufferFromInt(int value)
  {
    ByteBuffer b = ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN);
    b.putInt(value);
    b.flip();
    return b;
  }//end byteBufferFromInt

  private class SingleExecEntry
  {
    public CommandSender sender;
    public Command command;
  }//end SingleExecEntry

  public List<CommandSender> getListeners() {
    return listeners;
  }//end getListeners

  public long getReceivedBytes() {
    return receivedBytes;
  }//end getReceivedBytes

  public long getSentBytes() {
    return sentBytes;
  }//end getSentBytes
  
}//end class MessageServer
