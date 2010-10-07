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
import java.util.TreeMap;
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
  private final Lock SINGLE_EXE_LOCK = new ReentrantLock();
  
  public final static String STRING_ENCODING = "ISO-8859-15";
  private SocketChannel serverSocket;
  private Thread senderThread;
  private long updateIntervall = 60;
  private InetSocketAddress address;
  private List<CommandSender> listeners;
  private List<SingleExecEntry> singleExec;
  private Map<Integer, SingleExecEntry> answerRequestQueue;
  private int idCounter;
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
    idCounter = Integer.MIN_VALUE;
    serverSocket = null;
    this.parent = parent;
    this.receivedBytes = 0;
    this.sentBytes = 0;

    listeners = new LinkedList<CommandSender>();
    singleExec = new LinkedList<SingleExecEntry>();

    answerRequestQueue = new TreeMap<Integer, SingleExecEntry>();
  }

  public void connect(String host, int port) throws IOException
  {
    if(serverSocket != null && serverSocket.isConnected())
    {
      isActive = false;
      serverSocket.close();
    }

    senderThread = new Thread(new Runnable()
    {

      public void run()
      {
        sendReceiveLoop();
      }
    });


    address = new InetSocketAddress(host, port);

    serverSocket = SocketChannel.open(address);
    while(!serverSocket.finishConnect())
    {
      // wait
    }

    isActive = true;

    if(parent != null)
    {
      parent.showConnected(true);
    }
    senderThread.start();
  }//end connect

  public void disconnect() throws IOException
  {
    isActive = false;
    if(serverSocket != null && serverSocket.isConnected())
    {
      try
      {
        // cleanup
        ByteBuffer buffer = ByteBuffer.allocate(1);
        serverSocket.configureBlocking(false);
        while(serverSocket.read(buffer) > 0)
        {
          buffer.clear();
        }
      }
      catch(IOException ex)
      {
        // ignore
      }

      // call error handlers of remaining requests
      for(Map.Entry<Integer, SingleExecEntry> entry : answerRequestQueue.entrySet())
      {
        if(entry.getValue().sender != null)
        {
          entry.getValue().sender.handleError(-2);
        }
      }
      answerRequestQueue.clear();


      // disconnent
      serverSocket.close();
      serverSocket = null;

      // notifiy disconnect
      if(parent != null)
      {
        parent.showConnected(false);
      }
    }//end if
  }//end disconnect

  public InetSocketAddress getAddress()
  {
    return address;
  }//end getAddress

  /** Return wether RC is connected to a robot */
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
   * Schedule a single command for execution. It is not guarantied when it
   * will be executed but the {@link CommandSender} will be notfied using the
   * right command as argument.
   * 
   * @param commandSender The command sender responsible for this command.
   * @param command       Instead of using {@link CommandSender#getCurrentCommand()}
   *                      this command is used.
   * 
   * @throws NotYetConnectedException is thrown if the sercer is not connected
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

  
  // send-receive-loop //
  public void sendReceiveLoop()
  {
    while(isActive && serverSocket != null && serverSocket.isConnected())
    {
      try
      {
        try
        {
          long startTime = System.currentTimeMillis();

          pollAnswers();
          sendPendingCommands();
          while(answerRequestQueue.size() > 0)
          {
            pollAnswers();
            // HACK: prevent deadlock on single core machines
            // ...for Thomas to fix :)
            Thread.sleep(1);
          }
          long stopTime = System.currentTimeMillis();
          long diff = updateIntervall - (stopTime-startTime);
          long wait = Math.max(0, diff);
          if(wait > 0)
          {
            Thread.sleep(wait);
          }
        }
        catch(InterruptedException ex)
        {
          isActive = false;
          Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "thread was interupted", ex);
          disconnect();
        }
      }
      catch(IOException ex)
      {
        Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, null, ex);
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
        int id = sendSingleCommandReturnID(serverSocket, e.command);

        answerRequestQueue.put(id, e);
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

        
        int id = sendSingleCommandReturnID(serverSocket, command);

        SingleExecEntry e = new SingleExecEntry();
        e.command = command;
        e.sender = sender;
        answerRequestQueue.put(id, e);

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
      // read first 4 byte (id)
      ByteBuffer idBuffer = readContent(serverSocket, 4);
      if(idBuffer == null)
      {
        return;
      }
      somethingFound = true;
      
      int id = idBuffer.getInt(0);

      serverSocket.configureBlocking(true);

      // read size of data
      ByteBuffer sizeBuffer = readContent(serverSocket, 4);
      if(sizeBuffer == null)
      {
        return;
      }
      int size = sizeBuffer.getInt(0);

      // approximated size of received data
      receivedBytes += size;

      SingleExecEntry e = answerRequestQueue.get(id);

      if(e == null)
      {        
        // non-existing, skip message
        readContent(serverSocket, size);
      }
      else
      {
        // remove from map
        answerRequestQueue.remove(id);

        try
        {
          ByteBuffer data = readContent(serverSocket, size);
          // call sender
          if(e.sender != null)
          {
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
      }
    }//end while
  }//end pollAnswers


  // helpers
  private int sendSingleCommandReturnID(SocketChannel socket, Command command) throws IOException
  {
    byte[] commandAsByteArray = command.toByteArray();
    int intLength = commandAsByteArray.length;
    int id = idCounter++;

    if(idCounter == Integer.MAX_VALUE)
    {
      idCounter = Integer.MIN_VALUE;
      id = idCounter;
    }
    
    ByteBuffer bId = byteBufferFromInt(id);
    bId.flip();

    ByteBuffer bLength = byteBufferFromInt(intLength);
    bLength.flip();

    ByteBuffer bCommand = ByteBuffer.wrap(commandAsByteArray);

    // write id
    sentBytes += socket.write(bId);
    // write length    
    sentBytes += socket.write(bLength);
    // write data
    sentBytes += socket.write(bCommand);

    return id;
  }//end sendSingleCommandReturnID

  private ByteBuffer readContent(SocketChannel socket, int length) throws IOException
  {
    if(length < 0)
    {
      System.err.println("invalid message length: " + length);
      return null;
    }
    else if(length > 10485760) // more than 10 MB
    {
      System.err.println("Message size received bigger than 10 MB, this is too big! Will disconnect.");
      disconnect();
      return null;
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
    else
    {
      return null;
    }
  }//end readContent

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
